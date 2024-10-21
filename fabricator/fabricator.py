#Binary Opcodes (0 -> CFAB {Not GFAB})
"""
Bin  | Opc | File-Rep.
_____|_____|__________
0000 | SET | 00XXAA
0001 | MOV | 01XXYY
0010 | AND | 02XXYY
0011 | OR_ | 03XXYY
0100 | NOT | 04XX00
0101 | XOR | 05XXYY
0110 | EQU | 06XXYY
0111 | GRT | 07XXYY
1000 | LSS | 08XXYY
1001 | ADD | 09XXYY
1010 | SUB | 0AXXYY
1011 | MUL | 0BXXYY
1100 | DIV | 0CXXYY
1101 | MOD | 0DXXYY
1110 | ABS | 0EXX00
1111 | BRN | 0FXXYY
"""

#Functions
"""
SET 					 |	 set A = B 								|	Sets the value in reg A to constant B.
MOV 					 |   mov A B 								|	Moves the value in register A to register B.
AND						 |	 A and B 								|	If A and B.
OR						 |	 A or B 								|	If A or B.
NOT						 |	 not A {B unused}						|	If A is false.
XOR						 |	 A xor B 								|	If A or B individually, but not both. (XOR)
equalTo					 |	 A = B 									|	If A is equal to B.
greaterThan				 |	 A > B 									|	If A is GreaterThan B
lessThan				 |	 A < B 									|	If A is LessThan B
greaterThanOrEqualTo	 |	 not (A < B)							|	If A is GreaterThan B OR they are equal
lessThanOrEqualTo		 |	 not (A > B)							|	If A is LessThan B OR they are equal
invert					 |	 not (A) {for all bits}					|	Makes negative with 2s Compliment (All NOT, LSB = same)
add						 |	 A + B 									|	Adds B to A
subtract				 |	 A + (inv B)							|	Subtracts B from A
multiply				 |	 A * B 									|	Multiplies 2 values.
divide					 |	 A / B 									|	Divides 2 values; always rounds down.
modulo					 |	 A % B 									|	Gets remainder of 2 values DIV'd.
absolute				 |	 abs A {B unused}						|	Sets the MSB to 1. That's it.
sign					 |	 A / (abs A) {B unused}					|	Gets the sign of A (8 bit values, so MSB; written mathmatically for completeness.)
branch					 |	 brn A if B is 1 {Conditional}			|	Jumps to A if reg B is 1.
jump					 |	 brn A 1 {B unused} {Unconditional}		|	Jumps to a marker without a comparison
mark					 |	 :A {B unused}							|	Marker. Used to branch to.
exit 					 |   brn :_end 1 {Unconditional}			|	Exit immediately (Jump to end marker)
"""


opcodes = {
	"set": "0000", "mov": "0001", "and": "0010", "or": "0011",
	"not": "0100", "xor": "0101", "equ": "0110", "gtr": "0111",
	"lss": "1000", "add": "1001", "sub": "1010", "mul": "1011",
	"div": "1100", "mod": "1101", "abs": "1110", "brn": "1111"
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
			raise ValueError(f"Value {value} out of range [-128 - 128] for signed 8-bit integer")
	
		if value < 0:
			# Handle two's complement for negative values
			return format((1 << 8) + value, '08b')
		else:
			return format(value, '08b')
	else:
		if not (0 <= value < 256):
			raise ValueError(f"Value {value} out of range [0 - 256] for unsigned 8-bit integer")

		return format(value, '08b')


def convertAllToBin(operator, A, B):
	zero, one, rOP = toBin(0), toBin(1), toBin(255)
	
	match operator:
		case "set" | "mov" | "and" | "or" | "xor" | "equ" | "gtr" | "lss" | "add" | "sub" | "mul" | "div" | "mod":
			return (
				opcodes[operator] + toBin(A) + toBin(B),
			)
		case "not" | "abs":
			return (
				opcodes[operator] + toBin(A) + zero,
			)
		case "sgn":
			return (
				opcodes["abs"] + toBin(A) + zero,
				opcodes["div"] + toBin(A) + rOP,
			)

		case "inv":
			return (
				opcodes["sub"] + zero + toBin(A),
			)

		case "gte":
			return (
				opcodes["lss"] + toBin(A) + toBin(B),
				opcodes["not"] + rOP + zero,
			)
		case "lse":
			return (
				opcodes["gtr"] + toBin(A) + toBin(B),
				opcodes["not"] + rOP + zero,
			)
		case "jmp":
			return (
				opcodes["brn"] + toBin(A) + one,
			)
		case "brn":
			return (
				opcodes["brn"] + toBin(A) + toBin(B),
			)
		case "ext":
			return (
				opcodes["brn"] + toBin(markers["_end"]) + one,
			)
		case _:
			raise ValueError(f"Unknown operator; {operator}, {A}, {B}")


def convertValues(A):
	if A == "rop":
		return 255
	elif A.startswith("r"):
		return int(A.replace("r", ""))
	elif A.startswith("#"):
		return int(A.replace("#", ""))
	elif A.startswith(":"):
		return markers[A.replace(":", "")]
	else:
		return A

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

	elif operands[0] in ("ext", "lse", "gte", "inv", "sgn", "jmp",):
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
	filename = "testloop"
	with open(f"{filename}.cfab", "r") as CFABFile:
		readlines = CFABFile.readlines()
		partial_lines = [line.strip().lower() for line in readlines if not line.startswith("//")]
		lines = [line for line in partial_lines if line != ""]
		markers = {}

		for line in lines:
			if line.startswith(":"):
				markers[line.replace(":", "")] = lines.index(line)
		markers["_end"] = len(lines)-1


		fabricated = []
		for line in lines:
			if not line.startswith(":"):
				fabricated.extend(convertLine(line))
		
		combinedBytes = ""
		for hexInstruction in fabricated:
			combinedBytes += hexInstruction

	with open(f"{filename}.dat", "w") as outFile:
		outFile.write(combinedBytes)