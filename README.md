# CFAB-computer
_Simple mock-up 16 instruction assembly language with interpreter._

### Information;
- All instructions use register locations for A and B, unless it is SET or COL.
- All values stored in registers are -128 → 128 (signed 8 bit int)
- There are 256 registers to use. Screen is 8x8.

### Important Registers;
- `r0/r1` → Generally used by some of the data-processing functions, not reccomended to be assigned by user for risk of overwrites.
- `r250 ` → A register that is ALWAYS True.     → "rT"
- `r251 ` → A register that is ALWAYS False.    → "rF"
- `r252 ` → Pointer's X coordinate.             → "rX"
- `r253 ` → Pointer's Y coordinate.             → "rY"
- `r254 ` → Pointer's colour index.             → "rCOL"
- `r255 ` → Output from previous instruction.   → "rOP"

### Data-processing Instructions;
 _Name_   | _Shorthand_         | _Description_
- `SET` | A = #B                | Sets a register, A, to the immediate signed int 8, B.
- `MOV` | A ~ B                 | Moves the contents of register A to register B.
- `AND` | A & B                 | Logical A and B.
- `OR ` | A | B                 | Logical A or B.
- `NOT` | ! A                   | Logical not A.
- `XOR` | A ^ B                 | Logical A xor B.
- `EQU` | A == B                | If A is equal to B.
- `GRT` | A > B                 | If A is greater than B.
- `LSS` | A < B                 | If A is less than B.
- `GTE` | A >= B                | If A is greater than OR equal to B.
- `LSE` | A <= B                | If A is less than OR equal to B.
- `INV` | inv A                 | Inverts the value of A. (int -> -int)
- `ADD` | A + B                 | Adds the values of A and B.
- `SUB` | A - B                 | Subtracts B from A.
- `MUL` | A * B                 | Multiplies A and B.
- `DIV` | A / B                 | Divides A by B (rounds DOWN)
- `MOD` | A % B                 | Finds A Modulo B.
- `ABS` | abs A                 | Absolute value of A.
- `SGN` | sgn A                 | Sign of A.
- `BRN` | brn :marker condition | Branches if condition is true, to the line :marker is on.
- `JMP` | jmp :marker           | Jumps unconditionally to :marker.
- `EXT` | ext                   | Immediately exits the processing.


### Graphical Instructions;
 _Name_   | _Shorthand_   | _Description_
- `COL` | col cB        | Sets the current colour to colour index B (shown as c0, c1 etc.).
- `PTR` | ptr A B       | Moves the pointer to the position given (X:A, Y:B).
- `REC` | rec A B       | Draws a rectangle at TL position (A, B) with BR position PTR.
- `PIX` | pix A B       | Draws a pixel at the position (A, B).
- `LNE` | lne A B       | Draws a pixel from (A, B) to PTR.
- `CLR` | clr           | Clears the screen with the current set-colour.


### Colours;
_Used with COL, as c{hex}_
_Hex_   | _Name_      | _RGB_
- `0` | Black     | (0  , 0  , 0  )
- `1` | D.Red     | (128, 0  , 0  )
- `2` | D.Green   | (0  , 128, 0  )
- `3` | D.Yellow  | (128, 128, 0  )
- `4` | D.Blue    | (0  , 0  , 128)
- `5` | D.Magenta | (128, 0  , 128)
- `6` | D.Cyan    | (0  , 128, 128)
- `7` | D.Grey    | (128, 128, 128)
- `8` | L.Grey    | (187, 187, 187)
- `9` | L.Red     | (187, 0  , 0  )
- `A` | L.Green   | (0  , 187, 0  )
- `B` | L.Yellow  | (187, 187, 0  )
- `C` | L.Blue    | (0  , 0  , 187)
- `D` | L.Magenta | (187, 0  , 187)
- `E` | L.Cyan    | (0  , 187, 187)
- `F` | White     | (255, 255, 255)


### The Instruction-set;
_Name_    | _Hex_   | _Function_
- `SET` | `0`   | Sets register A to value B
- `MOV` | `1`   | Moves contents of register A to register B
- `AND` | `2`   | Logical A AND B
- `OR_` | `3`   | Logical A OR B
- `NOT` | `4`   | Logical NOT A
- `LSS` | `5`   | If A < B
- `EQU` | `6`   | If A == B
- `GRT` | `7`   | If A > B
- `ADD` | `8`   | Sum of A and B
- `SUB` | `9`   | Sum of A and -B
- `MUL` | `A`   | Product of A and B
- `DIV` | `B`   | A divided by B
- `ABS` | `C`   | Absolute value of A
- `BRN` | `D`   | Branch to instruction A if B is True.
- `REC` | `E`   | Draw rectangle at pos (A, B) and PTR Dimentions.
- `LNE` | `F`   | Draw line starting at pos (A, B) and ending at PTR Coordinates.
