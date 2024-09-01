# Machine - A Backend Generator for Compilers

This project try to provide a method to generate C/C++ source files to describe a backend's structure.

The machine (or backend) is supposed to be defined with a special text grammar.

The generator read text inputs and analysis by the grammar and then generate C/C++ source files if no error.

## Grammar

Let's write the grammar as `G` and input as `T` for short.

If there's a machine are going to define, denotes as `M`.

`M` is supposed to specify three things: register groups, memories and instructions.

Those three denote as `M.reg`, `M.mem`, `M.instr`.

To define the machine `M`, there's a keyword should be presented: `machine`.

Then an identifier should be provided as the machine's name.

The three properties should be defined in a `{` and `}` pair and follow the machine's name.

The properties' order doesn't matter. And every kind of properties can have more than one definition items.

Those properties definition items should also be led keywords: `register`, `memory` or `instruction`.

Every definition item's content should be bracketed with `{` and `}` and end with `;`.

For example, a definition of machine "abc" should be looks like:
```
machine {
    register ... { ... };
    memory ... { ... };
    memory ... { ... };
    instruction ... { ... };
    register ... { ... };
    instruction ... { ... };
    ...
};
```
Here `...` means some texts.

For every kind of properties, the grammar and contents are different.

### Register Group

A definition led by `register` is a register group of machine.

A register group should be assigned an identifier as its name, and also should provide a bit field.

A register group should have one or more register. Every register is a unique item.

A register definition item is composed by an identifier as register's name, a bit field and a code.

The bit field presents which bits would be modified if the register be involved a computation.

The code is the encoding form of register in instructions. It will be used in machine code generation.

an example of a register group would be like:
```
register ax [64-bit] {
    r~: [63-0] = 0x00,
    e~: [31-0] = 0x00,
    ~ : [15-0] = 0x00,
    ah: [15-8] = 0x04,
    al: [7-0]  = 0x00,
};
```
Here `~` is a special grammar of the project.It means the group's name, and `r~` means concat `r` with `~`.


## Source Code Generate

The C/C++ source files (i.e. this project's output) would provide a data structure which
contains those properties of the given machine in form of below:
### registers

Every register item in all register groups will generate a struct in this form:
```C
struct Register {
    uint32_t        reg_name;
    struct Bits     reg_bits;
    uint16_t        reg_code;
};
```

The higher 16 bits of `reg_name` presented the register group that it in's identity and lower 16 bits means it identity in the group.

The `reg_bits` stores the register's lowest bit offset and highest bit offset. And there has
```C
struct Bits {
    uint8_t     lowest;
    uint8_t     highest;
};
```
The `reg_code` is the encoding of the register in instructions. 
