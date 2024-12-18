## Quickstart with CFAB
Simply put, CFAB operates off of a few rules.
- Each instruction, if it gives a result (such as an addition operation) will return the result in register 31.
- Each value is confined to, signed or unsigned, 8 bits.
- Commands in a .cfab file may become multiple instructions in the final instructions data.


Immediate values are represented with the prefix `#`. For instance, `#2` is the value 2 and `#-3` is the value -3.
Registers are referenced via the prefix `r`. For instance, all results from operations are stored in `r31`.
Registers may have pre-set names, such as `r31` also being referenced via `rOP` (register-operation) for clarity.


A command in CFAB is 1 line. You cannot have multiple commands on a singular line.
For more information on the commands, see [this section](commands.md#basic-commands), but in short they each perform a simple action.
For example, `ADD #1 #2` would return the sum of 1 and 2 (3) in `r31`.


Comments use the C-style double slash `//`, and mark out a single line.

For instance;
`//This is a comment.`

However;
```
//
This is not a valid comment.
//
```

> [!WARNING]
> It is not reccomended to use `r0` for anything more than a basic program. This is due to some more advanced commands using `r0` as a temporary value-store.





___
## Basic Commands
In CFAB, there are over 30 pre-made commands to choose from; roughly half of those are basic commands.

### Mathematical Commands;
| Instruction Name | A | B | Description | Prefix Example | Infix Example |
| :---: | :---: | :---: | --- | --- | --- |
| `ADD` | int | int | Sum of A and B. | `ADD r0 #1` | `r0 + #1` |
| `SUB` | int | int | A subtract B. | `SUB r2 r1` | `r2 - r1` |
| `MUL` | int | int | Product of A and B. | `MUL #2 r4` | `#2 * r4` |
| `DIV` | int | int | Floor-division of 2 non-zero integers. | `DIV r2 #2` | `r2 / #2` |
| `MOD` | int | int | Modulo of A and B. Remainder of sorts. | `MOD r2 #3` | `r2 % #3` |
| `ABS` | int | N/A | Absolute value of A. | `ABS #2` | N/A |
| `SGN` | int | N/A | Sign of A. | `SGN r2` | N/A |
| `INV` | int | N/A | Inverts A (1 → -1, -1 → 1) | `INV r2` | N/A |


### Logical Commands;
| Instruction Name | A | B | Description | Prefix Example | Infix Example |
| :---: | :---: | :---: | --- | --- | --- |
| `AND` | bool | bool | Boolean A AND B | `AND r0 r1` | `r0 & r1` |
| `OR` | bool | bool | Boolean A OR B | `OR r2 r0` | `r2 \| r0` |
| `NOT` | bool | N/A | NOT A. | `NOT r4` | `! r4` |
| `XOR` | bool | bool | If A doesnt equal B. | `XOR r0 r2` | `r0 ^ r2` |
| `LSS` | any | any | If A is less than B (not equal to) | `LSS #2 r2` | `#2 < r2` |
| `LSE` | any | any | If A is less than or equal to B | `LSE #2 r2` | `#2 <= r2` |
| `EQU` | any | any | If A equals B. | `EQU r3 #2` | `r3 == #2` |
| `GTE` | any | any | If A is greater than or equal to B | `GTE r3 #0` | `r3 >= #0` |
| `GTR` | any | any | If A is greater than B (not equal to) | `GTR r3 #0` | `r3 > #0` |

> [!IMPORTANT]
> For Boolean instructions, 0 is treated as `False` and 1 is treated as `True`.




### Miscellaneous Commands;
| Instruction Name | A | B | Description | Prefix Example | Infix Example |
| :---: | :---: | :---: | --- | --- | --- |
| `SET` | reg | int | Sets a given register to a given value. | `SET r0 #2` | `r0 = #2` |
| `MOV` | reg | reg | Moves contents of 1 register to another. | `MOV r0 r1` | `r0 ~ r1` |
| `EXT` | N/A | N/A | Immediately stops execution. | `EXT` | N/A |

> [!IMPORTANT]
> For `SET`, A must be a register. For `MOV`, both A and B must be registers.

> [!TIP]
> For `SET`, B can be a register too; this copies the contents of B, to A.




___
## Advanced Commands
Other commands are, more often than not, pre-made chains of instructions within CFAB.

### Chain instructions;
| Instruction Name | A | B | Description | Prefix Example | Infix Example |
| :---: | :---: | :---: | --- | --- | --- |
| `BRN` | marker | bool | Jumps to instruction A, if B. | `BRN :marker r0` | N/A |
| `JMP` | marker | N/A | Unconditionally jumps to A. | `JMP :marker` | N/A |
| `RAMwrite` | int | int | Writes B to RAM address A. | `RAMwrite #12 r2` | N/A |
| `RAMread` | int | N/A | Reads RAM address A. | `RAMread #12` | N/A |


### Markers;
Markers are used as jump-points in CFAB. This removes the need for the user to memorise instruction numbers, and so-forth.
To use a marker, you do like so;
`:marker`
Where "marker" is the name you want to assign to it. Any single-word name is valid.
`:thisIsValid`
Later, to jump to that marker in a `BRN` or `JMP`, you would do;
`BRN :marker {condition}`
`JMP :marker`


### Aliases;
You can alias certain registers under other names. Some built-in aliases are like so;
| Alias | Register | Value |
| --- | :---: | :---: |
| `rOP` | `r31` | N/A |
| `True` | N/A | `#1` |
| `False` | N/A | `#0` |
The goal of aliases is to provide more readable code, in lieu of variable names.

To define your own custom aliases, the syntax is as follows;

`$aliasName @ register`
Where aliasName can be any single-word string, and register is, for example, `r0`.
To use a custom alias again later in the code, you must do like so;

`$aliasName`
In place of the register. For example;

`ADD r1 r2`
Could be rewritten as;
`$index @ r1`
`ADD $index r2`


### Macros;
Macros work somewhat like functions. However, every time they are called, they are "unpacked" into that space.
To define a macro, with 2 variable inputs;

```
def %exampleMacro $A $B
//For example, this macro could do some operation on the 2 variables like so;
//Mean of A and B
$A + $B
rOP / #2
end
```
This then creates a macro, with the contained code. Comments are discarded as they are not fabricated anyhow.
Macro names can be any continuous string. There can be any number of arguments, 0 is valid.
To call a macro, you use the syntax;

`%exampleMacro r0 r1`
Which would "unpack" into the definition, pasted in place of that line.
The result of the macro's execution will be the state of the registers after. Macros are simply a time-saving measure on the programmer's part.
For example, this code defines and adds 2 immediate values, then finds the mean, and then multiplies the mean by the initial values.
```
def %mean $A $B
//For example, this macro could do some operation on the 2 variables like so;
//Mean of A and B
$A + $B
rOP / #2
end

//... other code ...
r0 = #2
r1 = #3

r0 + r1

%mean r0 r1
rOP * r0
rOP * r1
```

The second half of this code (after the `%mean` definition) would unpack like so;
```
r0 = #2
r1 = #3

r0 + r1

$A + $B
rOP / #2

rOP * r0
rOP * r1
```
Where the contents of `%mean` replace the line that called it.

You are permitted to reference other macros within a macro, and can use aliases when calling a macro, or when definining it (Aliases are global)

> [!NOTE]
> Macros cannot exceed more than 32-deep, and also cannot create infinite loops (referencing itself in the chain)


### Manually interfacing with RAM
The built in chain-instructions of `RAMwrite` and `RAMread` do multiple things, in order to do so.

Addresses for RAM are obtained through 2, 8b registers acting as a singular, 16b register.

`r30` corresponds to the first 8 bits of the address (including MSB)

`r29` corresponds to the last 8 bits of the address (including LSB)

`RAMwrite` goes through the following instructions, for inputs A and B.
```
SET r30 addressHalfA
SET r29 addressHalfB
SET r28 B
SET r26 True
```
And `RAMread` performs these instructions;
```
SET r30 addressHalfA
SET r29 addressHalfB
SET r27 True
```
Where `addressHalfA` is the first 8 bits of the address, and `addressHalfB` is the last 8 bits.

> [!NOTE]
> `RAMread` writes the value returned from RAM, into `rOP`.
