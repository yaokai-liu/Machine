# xMachine - An Assembler Generator

This project provides a method to generate an assembler in C or binary executable file.

## Functions

Users can write a script of target instruction set architecture,
and use this project to generate a C library.

The C library generated contains these functions:

1.  `Array<uint32_t> *listRegisters(void)` to get register-ids of the current machine;
2.  `Array<uint32_t> *listMemoryModel(void)` to get memory-models of the current machine;
3.  `Register *getRegisterInfo(uint32_t id)` to get information of a register in the current machine by id;
4.  `Memory *getMemModelInfo(uint32_t id)` to get information of a memory-model in the current machine by id;
5.  `uint32_t ${instr}(Array<uint8_t> *, ...)` to encode an instruction and write into an array;
6.  `uint32_t emit_${instr}(Array<uint8_t> *, ...)` to emit an instruction and record registers' allocation;
7.  `uint32_t getCycles()` to get cycles count of instructions emitted till now;
8.  `void setCycles(uint32_t)` to set initial cycles count from now;
9.  `void usedRegister(uint32_t)` to mark a register as used;
10.  `void unusedRegister(uint32_t)` to mark a register as unused;
11.  `bool isUsed(uint32_t)` to query if the register is used.
12.  `Array<uint32_t> *getUnused(uint32_t)` to get a list of not allocated
     registers in the set.
13.  `uint32_t dumpRegAllocation(void *)` to dump the registers' allocation;
14.  `uint32_t loadRegAllocation(void *)` to load a registers' allocation;
15.  `void* getCurrentMachine(void)` to get the address of the current context machine;
16.  `void setCurrentMachine(void*)` to set the current context machine;

## Build and Dependence

This project using a python script to generate some source files.
And it is written in C and request standard of C23, built with cmake.

So please make sure the python and cmake in the PATH environment,
and your C compiler supports cstd-23.

Then run command:
```shell
cmake -B <cmake-output-dir> -S <this-project-root-dir> -DCMAKE_C_COMPILER=<your-compiler>
cmake build <cmake-output-dir> --target machine-c --parallel <your-cpu-core-count>
```

Without any error, there is a directory named `output` in the `<this-project-root-dir>`
and an executable file named `machine-c` in it.

Then write some script to experience it!

## Grammar of assembler script

The assembler is supposed to be defined with a special text grammar.

The generator read text inputs and analysis by the grammar and then generate a C header file and,
a source file or a library archive file if no error.

In the script, an assembler is called machine, cause every assembler is specified with a certain instruction set architecture,
i.e. a certain machine architecture.

If there's a machine are going defined, it is supposed to specify three kinds of things: **register groups**, **memories** and **instructions**.
To define the machine, there's a keyword should be presented: `machine`.
Then an identifier should be provided the machine's name.
The definitions of these three properties should be placed in a `{` and `}` pair follow the machine's name.
Every kind of properties should contain one or more definition items.
Those properties definition items should also be led by keywords: `register`, `memory` or `instruction`.
Every definition item's contents should be bracketed with `{` and `}` and ended with `;`.

For example, a definition of machine "abc" should be like:
```
machine abc {
    register ... { ... };
    memory ... { ... };
    memory ... {
        ...
    };
    instruction ... { ... };
    register ... { ... };
    instruction ... {
        ...
    };
    ...
};
```
(Here `...` means some texts.)

For every kind of properties, the grammar and contents are different.

#### Register Group

A RegisterGroup gives a description for registers of machine which share a same real location.

Every RegisterGroup should be led by a keyword `register`.
A RegisterGroup should be assigned an identifier as its name, and also should provide a [width](#width-bit-field-and-time-tick).
A RegisterGroup should have one or more register. Every register is a unique item.
A register definition item is composed by an identifier as register's name, a [bit field](#width-bit-field-and-time-tick) and a code.
The bit field presents which bits would be modified if the register be involved a computation.
The code is the encoding form of register in instructions. It will be used in machine code generation.
Write it in a more formal grammar:
```
<identifier> : <bit field> = <code> ;
```

An example of a RegisterGroup would be like:
```
register ax [64-bit] {
    rax: [63-0] = 0x00;
    eax: [31-0] = 0x00;
    ax : [15-0] = 0x00;
    ah : [15-8] = 0x04;
    al : [7-0]  = 0x00;
};
```

The text above defines a RegisterGroup named `ax` and full width is 64 bits.
In this RegisterGroup, there are 5 registers, named `rax`, `eax`, `ax`, `ah` and `al`.
For `rax`, it occupies all 0 to 63rd bit (total 64 bits) and will be encoded as `0x00` in instructions.
But for `ah`, it only takes 8 to 15th bit (total 8 bits) and will be encoded as `0x04` in instructions.
The others could be read in same way.

Therefore, it is easy to know that **registers in the same group will exclude each other**.

#### Memory

A Memory definition item gives actually the Memory addressing mode of a machine. It's led by the keyword `memory`.

A Memory definition item have and only have two fields: base and offset.
and these two field are directly expressed with two symbols `$` and `>`.
Both base and offset can be directly with registers or [bit field](#width-bit-field-and-time-tick).

A [width](#width-bit-field-and-time-tick) follows the name of memory is required.

For example:
```
memory local [12-bit] {
    $: [0-5];
    >: [6-12];
};
```
The text above defines a Memory addressing mode named `local`, which has total 12 bits, and its base stores lower 0 to 5th bit (total 6 bits) and its offset stores upper 6 to 12th bit (total 6 bits, too).

#### Instruction

An Instruction definition item provides the byte code layouts of an Instruction's machine code.

An Instruction definition item led by a keyword `instruction` and an identifier as its name.
Like other machine properties, contents of Instruction definition items should be bracketed with `{` and `}`.

A statement in an instruction definition item are called a **form** of the instruction.
A form consists with a [pattern](#pattern-of-instruction-form) and a description.
The pattern given arguments the form used and the description provides the width, the time cost and the main body of the form.

contents of a form body will be bracketed with `{` and `}`.
In the form body, a prefix can be set with `^` and a suffix can be set with `&`. And a required part of a form will be led with `~`.

A `:` should follow the part's leader and a [width](#width-bit-field-and-time-tick) should be provided.
Each part of the form body should provide a [bit field](#width-bit-field-and-time-tick) preceded by `=`, means the part's layout. 
In formal grammar, it would be
```
<part> : <width> = <layout> ;
```
The actual layout of these three parts can be numbers, variables, attributes, or bit field mappings.
For example, the
```
instruction foo {
    [aaa, bbb] = [10-byte] (4-tick) {
        ^: [8] = 0x12;
        &: [8] = aaa.xxx;
        ~: [32] = {
            [0-5] = 0x34,
            [31-24] = bbb[13-20],
            [22-11] = aaa.yyy,
            [...] = 0
        };
```
defines an Instruction `foo` with only one form. The form in binary will be like
```
+--------+------+-----+------------+-+--------+--------+
|00010010|110100|00000|yyyyyyyyyyyy|0|bbbbbbbb|xxxxxxxx|
+--------+------+-----+------------+-+--------+--------+
   0x12    0x34          aaa.yyy        bbb     aaa.xxx
```
and it will cost 4 cpu [time tick](#width-bit-field-and-time-tick) for executing.


#### Immediate

An Immediate provides a type data with fixed width.

Every Immediate should start with a keyword `immediate`.
And an identifier follows as its name.
A [width](#width-bit-field-and-time-tick) must be provided for an immediate. 
Then an optional field can be set as its type.
The type can be one of the following table:

|   type   |      means       |
|:--------:|:----------------:|
| unsigned | unsigned integer |


Example:
```
immediate refer [23-bit] unsigned;
```

Immediate only used in Sets or Instructions.

#### Set

A Set is a special collection of registers, memories or Immediate.

Every Set definition item starts with a keyword `set` and an identifier as its name.
Its contents are bracketed with `{` and `}`, and seperated with commas.
Every element of a Set is a Register, Memories or Immediate.
And all elements of a Set should be items of same type. 

For example
```
set reg { rax, rbx, rcx };
set mem { local, short, long };
set imm { offset, index, bimp };
```

Sets only used in Instructions.

### Appendix

#### width, bit field and time tick

A width is a number with data units and bracketed with `[` and `]`. 
The data units can be `bit` or `byte`.
The data units can be omitted. When data unit omitted, the default data unit is `bit`.
If the data unit not omitted, the data unit should be contacted with `-`.
For examples

```
[16-bit], [7-byte], [32]
```

A bit field are two number contacted with `-`.
These two number are called left(highest) bound and right(lowest) bound of the bit field.
The range of bit field contains its bounds.
For example
```
[1-2], [23-55], [9-9]
```

A time tick is a number with time units and bracketed with `(` and `)`.
The time units can only be `tick` means one CPU's clock cycle.
Examples:
```
(1-tick), (3-tick)
```

#### table of macros

There's a lot of symbols as macro to terse the grammar.

| symbol |    where    |     means      |
|:------:|:-----------:|:--------------:|
|  `^`   | Instruction |     prefix     |
|  `&`   | Instruction |     suffix     |
|  `~`   | Instruction | principal part |
|  `$`   |   Memory    |      base      |
|  `>`   |   Memory    |     offset     |


#### identifier

An identifier is a text string only consists with letters and digits.

#### pattern of instruction form

A pattern of an InstructionForm is consist of identifiers seperated with comma and bracketed with `[` and `]`.
Items of a pattern called arguments or parameters.

Example
```
[ax, local, bx]
```
the order of the parameters doesn't matter.

#### total grammar

```
<Machine>           ->      machine     <identifier> { ... } ;
<RegisterGroup>     ->      register    <identifier> { <Register>+ };
<Memory>            ->      memory      <identifier> { <Base> ; <Offset> ; } ;
<Instruction>       ->      instruction <identifier> { <InstructionForm>+ } ;
<Immediate>         ->      immediate   <identifier> <width> <type> ;
<Set>               ->      set         <identifier> { <identifier>+ } ;

<Register>          ->      <identifier> : <width> = <number> ;

<Base>              ->      $ : <width> ;
<Offset>            ->      > : <width> ;

<InstructionForm>   ->      <Pattern> = <width> <time-tick>? { <Part>!3 } ;
<Patten>            ->      [ <idnetifier>+ ]
<Part>              ->      <part> : <width> = <Layout> ;
<Layout>            ->      <evaluable>
<Layout>            ->      <Mapping>
<Mapping>           ->      { <MappingItem>+ }
<MappingItem>       ->      <bit-field> = <evaluable>
```

and

```
<evaluable>       ->        <identifier> . <identifier>
<evaluable>       ->        <identifier> <bit-feild>
<evaluable>       ->        <identifier>
<evaluable>       ->        <number>
```

## TODO:

Codegen/C:

1. error messages;
2. machine model;
3. query functions;
4. test work.