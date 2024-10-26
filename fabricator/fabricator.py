#Binary Opcodes (Processing)
"""
Bin  | Opc | File-Rep.
_____|_____|__________
0000 | SET | 0XXAA
0001 | MOV | 1XXYY
0010 | AND | 2XXYY
0011 | OR_ | 3XXYY
0100 | NOT | 4XX00
0101 | LSS | 5XXYY
0110 | EQU | 6XXYY
0111 | GRT | 7XXYY
1000 | ADD | 8XXYY
1001 | SUB | 9XXYY
1010 | MUL | AXXYY
1011 | DIV | BXXYY
1100 | ABS | CXX00
1101 | BRN | DXXYY
1110 | REC | EXXYY
1111 | LNE | FXXYY
"""

#Processing Functions
"""
SET 		|	 set A = B 							|	Sets the value in reg A to constant B.
MOV 		|    mov A B 							|	Moves the value in register A to register B.
AND			|	 A and B 							|	If A and B.
OR			|	 A or B 							|	If A or B.
NOT			|	 not A {B unused}					|	If A is false.
XOR			|	 A xor B 							|	If A or B individually, but not both. (XOR)
EQU			|	 A = B 								|	If A is equal to B.
GRT			|	 A > B 								|	If A is GreaterThan B
LSS			|	 not (A > B or A == B)				|	If A is LessThan B
GTE			|	 not (A < B)						|	If A is GreaterThan B OR they are equal
LSE			|	 not (A > B)						|	If A is LessThan B OR they are equal
INV			|	 not (A) {for all bits}				|	Makes negative with 2s Compliment (All NOT, LSB = same)
ADD			|	 A + B 								|	Adds B to A
SUB			|	 A + (inv B)						|	Subtracts B from A
MUL			|	 A * B 								|	Multiplies 2 values.
DIV			|	 A / B 								|	Divides 2 values; always rounds down.
MOD			|	 A - ((A / B) * B)					|	Gets remainder of 2 values DIV'd.
ABS			|	 abs A {B unused}					|	Sets the MSB to 1. That's it.
SGN			|	 A / (abs A) {B unused}				|	Gets the sign of A (8 bit values, so MSB; written mathmatically for completeness.)
BRN			|	 brn A if B is 1 {Conditional}		|	Jumps to A if reg B is 1.
JMP			|	 brn A 1 {B unused} {Unconditional}	|	Jumps to a marker without a comparison
EXT 		|    brn :_end 1 {Unconditional}		|	Exit immediately (Jump to end marker)
{marker}	|	 :A {B unused}						|	Marker. Used to branch to.
"""

#Graphics Functions
"""
COL 	|	 mov A r254				|	Colour changer.
PTR 	|	 mov A r252; mov A r253	|	Pointer, with coordinates.
REC 	|	 rec A B				|	Rectangle.
PIX 	|	 pnt A B; rec A B		|	Singular Pixel.
LNE 	|	 dim A B; lne A B		|	Line between 2 points.
CLR 	|	 pnt 8 8; rec 0 0 		|	Clear screen.
"""


opcodes = {
	"set": "0000", "mov": "0001", "and": "0010", "or": "0011",
	"not": "0100", "lss": "0101", "equ": "0110", "gtr": "0111",
	"add": "1000", "sub": "1001", "mul": "1010", "div": "1011",
	"abs": "1100", "brn": "1101", "rec": "1110", "lne": "1111"
}

infixOperatorsList = {
	"+": "add", "-": "sub", "*": "mul", "/": "div", "%": "mod",
	"!": "not", "&": "and", "|": "or", "^": "xor",
	">": "gtr", "<": "lss", ">=": "gte", "<=": "lse", "==": "equ",
	"=": "set", "~": "mov",
}


def toBin(value, signed=False):
	if signed:
		if not (-128 <= value < 128):
			raise ValueError(f"Value {value} out of range [-128 - 127] for signed 8-bit integer")
	
		if value < 0:
			# Handle two's complement for negative values
			return format((1 << 8) + value, '08b')
		else:
			return format(value, '08b')
	else:
		if not (0 <= value < 256):
			raise ValueError(f"Value {value} out of range [0 - 256] for unsigned 8-bit integer")

		return format(value, '08b')


def SET(register, value):
	return opcodes["set"] + register + toBin(int(value) + 128)


def convertAllToBin(operator, A, B):
	width, height, blank = toBin(24), toBin(16), "0000"
	zero, one, rT, rF, rX, rY, rCol, rOP = toBin(0), toBin(1), toBin(250), toBin(251), toBin(252), toBin(253), toBin(254), toBin(255)
	
	match operator:
		case "mov" | "and" | "or" | "lss" | "equ" | "gtr" | "add" | "sub" | "mul" | "div" | "rec" | "lne":
			return (
				opcodes[operator] + toBin(A) + toBin(B),
			)

		case "set":
			return (
				SET(toBin(A), B),
			)

		case "not" | "abs":
			return (
				opcodes[operator] + toBin(A) + blank,
			)

		case "sgn":
			return (
				opcodes["abs"] + toBin(A) + blank,
				opcodes["div"] + toBin(A) + rOP,
			)

		case "inv":
			return (
				opcodes["sub"] + rF + toBin(A),
			)

		case "mod":
			return (
				opcodes["div"] + toBin(A) + toBin(B),
				opcodes["mul"] + rOP + toBin(B),
				opcodes["sub"] + toBin(A) + rOP,
			)

		case "xor":
			return (
				opcodes["equ"] + toBin(A) + toBin(B),
				opcodes["not"] + rOP + blank,
			)

		case "gte":
			return (
				opcodes["lss"] + toBin(A) + toBin(B),
				opcodes["not"] + rOP + blank,
			)

		case "lse":
			return (
				opcodes["gtr"] + toBin(A) + toBin(B),
				opcodes["not"] + rOP + blank,
			)

		case "jmp":
			return (
				opcodes["brn"] + toBin(A) + rT,
			)

		case "brn":
			return (
				opcodes["brn"] + toBin(A + 1) + toBin(B),
			)

		case "ext":
			return (
				opcodes["brn"] + toBin(markers["_end"]) + rT,
			)

		case "col":
			return (
				SET(rCol, A),
			)

		case "ptr":
			return (
				opcodes["mov"] + toBin(A) + rX,
				opcodes["mov"] + toBin(B) + rY,
			)

		case "pix":
			return (
				SET(rX, 1),
				SET(rY, 1),
				opcodes["rec"] + toBin(A) + toBin(B),
			)

		case "clr":
			return (
				SET(rX, width),
				SET(rY, height),
				opcodes["rec"] + zero + zero,
			)

		case _:
			raise ValueError(f"Unknown operator; {operator}, {A}, {B}")


def convertValues(A):
	if A == "rt":
		return 250
	elif A == "rt":
		return 251
	elif A == "rx":
		return 252
	elif A == "ry":
		return 253
	elif A == "rcol":
		return 254
	elif A == "rop":
		return 255
	elif A.startswith("r"):
		return int(A.replace("r", ""))
	elif A.startswith("#"):
		return int(A.replace("#", ""))
	elif A[0] == "c":
		return int(A[1:], 16)
	elif A.startswith(":"):
		return markers[A.replace(":", "")]
	else:
		return str(A) #Ensure the fallback is a string.

def convertLine(line):
	operands = line.split(" ")
	operands = [operand.strip() for operand in operands if operand != ""]
	if len(operands) == 2:
		operands.append("0")
	elif len(operands) == 1:
		operands.extend(["0", "0"])

	if operands[0] in opcodes:
		#Postfix
		operator, A, B = operands

	elif operands[1] in infixOperatorsList:
		#Infix
		A, operator, B = operands
		operator = infixOperatorsList[operator]

	elif operands[0] in ("ext", "lse", "gte", "inv", "sgn", "jmp", "col", "pix", "clr", "ptr"):
		#Chained or unusual operators
		operator, A, B = operands

	elif operands[0] in ("!",):
		operator, A, B = operands
		operator = infixOperatorsList[operator]

	else:
		raise ValueError(f"Unknown values; {operands}")

	A = convertValues(A)
	B = convertValues(B)

	instructionList = convertAllToBin(operator, A, B)
	hexList = [f"{int(instruction, 2):05X}" for instruction in instructionList]
	return hexList



if __name__ == "__main__":
	filename = "testgraphics"
	with open(f"{filename}.cfab", "r") as CFABFile:
		readlines = CFABFile.readlines()
		partial_lines = [line.strip().lower() for line in readlines if not line.startswith("//")]
		lines = [line for line in partial_lines if line != ""]
		markers = {}

		for curLine in lines:
			if curLine.startswith(":"):
				markers[curLine.replace(":", "")] = [accLine for accLine in lines if ((not (accLine.startswith(":") or accLine == "" or accLine.startswith("//"))) or accLine == curLine)].index(curLine)+1
		markers["_end"] = len(lines)-1


		preInstructions = []
		preInstructions.extend(convertAllToBin("set", 250, 1)) #rT
		preInstructions.extend(convertAllToBin("set", 251, 0)) #rF
		fabricated = [f"{int(instruction, 2):05X}" for instruction in preInstructions]
		for line in lines:
			if not line.startswith(":"):
				fabricated.extend(convertLine(line))
		
		combinedBytes = ""
		for hexInstruction in fabricated:
			combinedBytes += hexInstruction

	with open(f"{filename}.dat", "w") as outFile:
		outFile.write(combinedBytes)