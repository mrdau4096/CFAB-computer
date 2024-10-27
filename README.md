# CFAB-Interpreter
_Simple mock-up 16 instruction assembly language with interpreter._

### General Information;
- All instructions use register locations for A and B, unless it is SET or COL.
- All values stored in registers are -128 → 128 (signed 8 bit int)
- There are 256 registers to use. Screen & frameBuffer are 24x16.
- CFAB Files have a 65,536 instruction limit (past that value, JMP, BRN and EXT stop functioning correctly.)
- 1 CFAB Command ≠ 1 Instruction.

### Important Registers;
| Index | Alias | Description |
| :---: | :---: | --- |
| `r0 & r1` | `N/A` | Generally used by some of the data-processing functions,not reccomended to be assigned by user for risk of overwrites.
| `r248 r249` | `N/A` | Used in combination as 2 unsigned 8-bit integers, to become a singular unsigned 16-bit integer instruction to jump to.
| `r250 ` | `rT` | A register that is ALWAYS True.
| `r251 ` | `rF` | A register that is ALWAYS False.
| `r252 ` | `rX` | Pointer's X coordinate.
| `r253 ` | `rY` | Pointer's Y coordinate.
| `r254 ` | `rCol` | Pointer's colour index.
| `r255 ` | `rOP` | Output from previous instruction.

### Data-processing Commands;
| _Name_ | _Shorthand_ | _Description_ |
| :---: | :---: | --- |
| `SET` | `A = #B` | Sets a register, A, to the immediate signed 8-bit integer, B. |
| `MOV` | `A ~ B` | Moves the contents of register A to register B. |
| `AND` | `A & B` | Logical A and B. |
| `OR ` | `A | B` | Logical A or B. |
| `NOT` | `! A` | Logical not A. |
| `XOR` | `A ^ B` | Logical A xor B. |
| `EQU` | `A == B` | If A is equal to B. |
| `GRT` | `A > B` | If A is greater than B. |
| `LSS` | `A < B` | If A is less than B. |
| `GTE` | `A >= B` | If A is greater than OR equal to B. |
| `LSE` | `A <= B` | If A is less than OR equal to B. |
| `INV` | `inv A` | Inverts the value of A. (int -> -int) |
| `ADD` | `A + B` | Adds the values of A and B. |
| `SUB` | `A - B` | Subtracts B from A. |
| `MUL` | `A * B` | Multiplies A and B. |
| `DIV` | `A / B` | Divides A by B (rounds DOWN) |
| `MOD` | `A % B` | Finds A Modulo B. |
| `ABS` | `abs A` | Absolute value of A. |
| `SGN` | `sgn A` | Sign of A. |
| `BRN` | `brn :marker A` | Branches if A is true, to the line :marker is on. |
| `JMP` | `jmp :marker` | Jumps unconditionally to :marker. |
| `EXT` | `ext` | Immediately` exits the processing. |


### Helper Commands;
| _Name_ | _Shorthand_ | _Description_ |
| :---: | :---: | --- |
| `N/A` | `:marker` | Creates a marker to jump to, at the current line. Jumping to this will execute the following line, onward. |
| `DEF` | `def %macroName {args}` | Creates a macro with name macroName, taking in {args} (can be aliases). |
| `END` | `end` | Ends the definition of a macro. |
| `N/A` | `%macroName {args}` | Calls %macroName (all functions within macroName get added here. Result is the end state of %macroName) |


### Graphical Commands;
| _Name_ | _Shorthand_ | _Description_ |
| :---: | :---: | --- |
| `COL` | `col cB` | Sets the current colour to colour index B (shown as c0, c1 etc.). |
| `PTR` | `ptr A B` | Moves the pointer to the position given (X:A, Y:B). |
| `REC` | `rec A B` | Draws a rectangle at TL position (A, B) with BR position PTR. |
| `PIX` | `pix A B` | Draws a pixel at the position (A, B). |
| `LNE` | `lne A B` | Draws a pixel from (A, B) to PTR. |
| `CLR` | `clr` | Clears the screen with the current set-colour. |


### Colours;
_Used with COL, as_ `COL c{hex}`
| _Hex_ | _Name_ | _RGB_ |     | _Hex_ | _Name_ | _RGB_ |
| :---: | :--- | :---: | --- | :---: | :--- | :---: |
| `0` | Black | `(0  , 0  , 0  )` | | `8` | Light Grey | `(187, 187, 187)` |
| `1` | Dark Red | `(128, 0  , 0  )` | | `9` | Light Red | `(187, 0  , 0  )` |
| `2` | Dark Green | `(0  , 128, 0  )` | | `A` | Light Green | `(0  , 187, 0  )` |
| `3` | Dark Yellow | `(128, 128, 0  )` | | `B` | Light Yellow | `(187, 187, 0  )` |
| `4` | Dark Blue | `(0  , 0  , 128)` | | `C` | Light Blue | `(0  , 0  , 187)` |
| `5` | Dark Magenta | `(128, 0  , 128)` | | `D` | Light Magenta | `(187, 0  , 187)` |
| `6` | Dark Cyan | `(0  , 128, 128)` | | `E` | Light Cyan | `(0  , 187, 187)` |
| `7` | Dark Grey | `(128, 128, 128)` | | `F` | White | `(255, 255, 255)` |


### The Instruction-set;
| _Name_ | _Hex_ | _Function_ |    | _Name_ | _Hex_ | _Function_ |
| :---: | :---: | --- | --- | :---: | :---: | --- |
| `SET` | `0` | Sets register A to value B | | `ADD` | `8` | Sum of A and B |
| `MOV` | `1` | Moves contents of register A to register B | | `SUB` | `9` | Sum of A and -B |
| `AND` | `2` | Logical A AND B | | `MUL` | `A` | Product of A and B |
| `OR` | `3` | Logical A OR B | | `DIV` | `B` | A divided by B |
| `NOT` | `4` | Logical NOT A | | `ABS` | `C` | Absolute value of A |
| `LSS` | `5` | If A < B | | `BRN` | `D` | Branch to instruction A if B is True. |
| `EQU` | `6` | If A == B | | `REC` | `E` | Draw rectangle at pos (A, B) and PTR Dimentions. |
| `GRT` | `7` | If A > B | | `LNE` | `F` | Draw line starting at pos (A, B) and ending at PTR Coordinates. |
