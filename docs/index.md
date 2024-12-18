# CFAB Documentation
___
## Sections
- [Intro to CFAB](index.md#intro-to-cfab)
- [QuickStart with CFAB](commands.md#quickstart-with-cfab)
- [The instruction set](information.md#instruction-set)
- [Basic CFAB Commands](commands.md#basic-commands)
- [Advanced CFAB Commands](commands.md#advanced-commands)
- [Implementation Specifics](information.md#implementation-specifics)
___
## Intro to CFAB
CFAB is a mockup of an assembly-like programming language. Made to run on a custom interpreter (Written in CPP) or used within some of my other projects.
The main premise behind CFAB is a simplistic language, made for dealing with 8-bit values, 32 registers, a larger RAM section and so forth. There only 16 instructions in the instruction-set, so some commands written in CFAB are fabricated (similar to being assembled, or compiled) into a chain of sub-instructions. CFAB instructions are executed sequentially, and the results from any operation are stored in register 31 (if zero-indexed, as CFAB does.)

### Why not use another language?
I chose to create CFAB as a personal challenge, and to learn more about how instruction sets, and assembly-like programming, function. CFAB is reasonably basic, and has limited functionality, but is nonetheless capable enough for simple tasks.
