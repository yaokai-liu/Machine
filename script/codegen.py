import os.path
import sys
from pathlib import Path
import json
from string import Template as Tp
from DATA import *
import re
from functools import cmp_to_key as c2k


class Rule:
    def __init__(self, name, generator):
        self.name = name
        target, items = generator.rules[name].split('->')
        self.target = target.strip()
        if self.target == '~':
            self.target = generator.GRAMMAR_TARGET
        items = items.strip()
        if len(items) > 0:
            self.items = items.split(' ')
        else:
            self.items = []


class Action:
    def __init__(self, p, generator):
        if not p.startswith('('):
            rule = Rule(p, generator)
            self.action = "reduce"
            self.type = f"enum_{rule.target}"
            self.count = len(rule.items)
            self.offset = f"{p}"
        else:
            self.action = "stack"
            self.type = 0
            self.count = 0
            self.offset = f"{generator.state_to_enum(p)[0]}"

    def __eq__(self, other):
        return self.action == other.action \
            and self.type == other.type \
            and self.count == other.count \
            and self.offset == other.offset

    def to_string(self):
        return ("{"
                f".action = {self.action},"
                f".type = {self.type},"
                f".count = {self.count},"
                f".offset = {self.offset},"
                "}")


class Generator:
    def __init__(self, json_dir: Path | str, template_dir: Path | str, out_dir: Path | str, target: str):
        self.JSON_DIR = Path(json_dir)
        self.TEMPLATE_DIR = Path(template_dir)
        self.OUT_DIR = Path(out_dir)
        self.GRAMMAR_TARGET = target

        self.__tokens = self.get_json_from("tokens.json")
        self.terminals = sorted(['TERMINATOR'] + self.__tokens['terminal'])
        self.targets = self.__tokens['non-terminal']
        self.targets.remove("~")
        self.targets = sorted(self.targets)
        self.tokens = sorted(self.terminals + self.targets)
        if os.path.isfile(self.JSON_DIR / "machine-compact.json"):
            self.table = self.get_json_from("machine-compact.json")
        else:
            self.table = self.get_json_from("machine.json")
        for val in self.table.values():
            if '$' in val.keys(): val['TERMINATOR'] = val.pop('$')
        self.rules = self.get_json_from("rules.json")
        self.status, self.reflect = dict(), dict()
        for s, p in enumerate(self.table.keys()):
            self.status[s], self.reflect[p] = self.table[p], s
        self.extend_tokens = self.tokens
        self.context = "void"

    def set_extend_tokens(self, tokens):
        self.extend_tokens = tokens

    def set_context(self, context: str):
        self.context = context

    def get_json_from(self, filename: str):
        with open(self.JSON_DIR / filename, 'r') as fp:
            return json.load(fp)

    def get_temp_from(self, filename: str):
        with open(self.TEMPLATE_DIR / filename, 'r') as fp:
            return fp.read()

    def gen_terminals(self):
        template = Tp(self.get_temp_from("terminal.c.tpl"))
        body = ',\n  '.join([f'enum_{t}' for t in self.terminals if TERMINALS[t] != 0])
        strings = ',\n  '.join([f'[enum_{t}] = string_t("{TERMINALS[t]}")' for t in self.terminals if TERMINALS[t] != 0])
        string_lens = ',\n  '.join([f'[enum_{t}] = {len(TERMINALS[t])}' for t in self.terminals if TERMINALS[t] != 0])
        terminals_entry = template.substitute(strings=strings, string_lens=string_lens, terminals=body)
        with open(self.OUT_DIR / "terminal.gen.c", 'w') as fp:
            fp.write(terminals_entry)

    @staticmethod
    def state_to_enum(p):
        p = p.strip('()').split(', ')
        _state = '_'.join(p)
        current = 'TERMINATOR' if len(p) == 1 and p[0] == '' else p[-1]
        _state = ('__' + _state) if _state else '__EMPTY__'
        return _state, current

    def gen_reduces(self):
        rule_names = self.rules.keys()
        args = f"(void * argv[], {self.context} *, const Allocator * allocator);"
        enum_reduces = sorted(f"{r} = {i}" for i, r in enumerate(rule_names))
        reduces = sorted(f"{re.sub(r'_\d+$', '', r)} * p_{r}" + args
                         if r != '__EXTEND_RULE__'
                         else f"{self.GRAMMAR_TARGET} * p__{self.GRAMMAR_TARGET}__" + args
                         for r in rule_names)
        assign_reduces = sorted([f"[{r}] = (fn_reduce *) p_{r}" if r != '__EXTEND_RULE__'
                                 else f"[{r}] = (fn_reduce *) p__{self.GRAMMAR_TARGET}__"
                                 for r in rule_names])
        template = Tp(self.get_temp_from("reduce.h.tpl"))
        content = template.substitute(
            enum_reduces=',\n  '.join(enum_reduces),
            reduces='\n'.join(reduces)
        )
        with open(self.OUT_DIR / "reduce.gen.h", 'w') as fp:
            fp.write(content)
        content = Tp(self.get_temp_from("target.c.tpl")).substitute(
            assign_reduces=',\n  '.join(assign_reduces)
        )
        with open(self.OUT_DIR / "target.gen.c", 'w') as fp:
            fp.write(content)


    def gen_action_table(self):
        def token_cmp(t1, t2):
            a = self.extend_tokens.index(t1)
            b = self.extend_tokens.index(t2)
            return 1 if a > b else 0 if a == b else -1
        state_enum, states, actions, jumps, units, currents = [], [], [], [], [], []
        for p, q in self.table.items():
            _state, current = self.state_to_enum(p)
            state_enum.append(f'{_state} = {len(state_enum)}')
            _tokens = q.keys()
            _terminals = sorted(_tokens & set(self.terminals), key=c2k(token_cmp))
            _targets = sorted(_tokens & set(self.targets), key=c2k(token_cmp))
            state = {
                "ndx_base": len(actions),
                "goto_base": len(jumps),
                "token_base": len(units),
                "n_tokens": len(_tokens),
            }
            items, ndx, addend = dict(), [], []
            for t in _terminals:
                act = Action(q[t], self)
                if act not in addend:
                    addend.append(act)
                items[t] = addend.index(act)
            actions += [a.to_string() for a in addend]
            for i, t in enumerate(_targets):
                jumps.append(f"{self.state_to_enum(q[t])[0]}")
                items[t] = i
            for i, t in enumerate(sorted(_tokens)):
                ndx.append(str(i))
                units.append(f"{{.type = enum_{t}, .offset = {items[t]}}}")
            string = ', '.join([f".{k} = {v}" for k, v in state.items()])
            states.append(f"[{_state}] = {{{string}}}")
            currents.append(f"[{_state}] = enum_{current}")

        template = Tp(self.get_temp_from("action-table.c.tpl"))
        content = template.substitute(
            actions=",\n  ".join(actions),
            jumps=", ".join(jumps),
            units=", \n  ".join(units),
            states=",\n  ".join(states),
            currents=",\n  ".join(currents),
        )
        with open(self.OUT_DIR / "action-table.gen.c", 'w') as fp:
            fp.write(content)
        content_h = Tp(self.get_temp_from("action-table.h.tpl")).substitute(
            state_enum=',\n  '.join(state_enum)
        )
        with open(self.OUT_DIR / "action-table.gen.h", 'w') as fp:
            fp.write(content_h)

    def generate(self):
        self.gen_terminals()
        self.gen_reduces()
        self.gen_action_table()

def gen_token_enum(template, tokens, out):
    with open(template, 'r') as fp:
        temp = fp.read()
    template = Tp(temp)
    enums = ',\n  '.join([f"enum_{t} = {i + 1}" for i, t in enumerate(tokens)])
    enums += ',\n  ' + f'MAX_REAL_TOKEN = {len(tokens) + 1}'
    enums_entry = template.substitute(enums=enums)
    with open(out, 'w') as fp:
        fp.write(enums_entry)
def gen_token_name(template, tokens, out):
    with open(template, 'r') as fp:
        temp = fp.read()
    template = Tp(temp)
    names = ',\n  '.join([f'[enum_{t}] = string_t("{t}")' for t in tokens])
    names_entry = template.substitute(names=names)
    with open(out, 'w') as fp:
        fp.write(names_entry)



if __name__ == '__main__':
    JSON_DIR = Path(sys.argv[1])
    TEMPLATE_DIR = Path(sys.argv[2])
    OUT_DIR = Path(sys.argv[3])

    GMachine = Generator(json_dir=JSON_DIR / "machine",
                         template_dir=TEMPLATE_DIR / "machine",
                         out_dir=OUT_DIR / "machine",
                         target="Machine")
    GMacro = Generator(json_dir=JSON_DIR / "macro",
                       template_dir=TEMPLATE_DIR / "macro",
                       out_dir=OUT_DIR / "macro",
                       target="MacroEntry")

    tokens = sorted(set(GMachine.tokens) | set(GMacro.tokens))
    GMachine.set_extend_tokens(tokens)
    GMacro.set_extend_tokens(tokens)

    gen_token_enum(TEMPLATE_DIR / "tokens.h.tpl",
                   tokens,
                   OUT_DIR / "tokens.gen.h")

    gen_token_name(TEMPLATE_DIR / "tokens.c.tpl",
                   tokens,
                   OUT_DIR / "tokens.gen.c")

    GMachine.set_context("ParseContext")
    GMacro.set_context("MacroContext")
    GMachine.generate()
    GMacro.generate()
