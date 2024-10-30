#Binary Opcodes (Processing)
"""
Bin  | Opc | Hex-Rep.
_____|_____|__________
0000 | NOP | 00000
0001 | SET | 1XXAA
0010 | MOV | 2XXYY
0011 | AND | 3XXYY
0100 | OR_ | 4XXYY
0101 | NOT | 5XX00
0110 | LSS | 6XXYY
0111 | EQU | 7XXYY
1000 | GRT | 8XXYY
1001 | ADD | 9XXYY
1010 | SUB | AXXYY
1011 | MUL | BXXYY
1100 | DIV | CXXYY
1101 | ABS | DXX00
1110 | BRN | EXXYY
1111 | UPD | FXXYY
"""

#Processing Functions
"""
SET 		|	 set A = B 							|	Sets the value in reg A to constant B.
MOV 		|	 mov A B 							|	Moves the value in register A to register B.
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
EXT 		|	 brn :_end 1 {Unconditional}		|	Exit immediately (Jump to end marker)
{marker}	|	 :A {B unused}						|	Marker. Used to branch to.
DEF/END	 	|	def %macroName {args}; ...; end 	|	Used to define a macro. Contents of macro added wherever called. Takes {args}.
{macro call}| 	 %macroName {args} 					|	Calls a pre-defined macro. Contents of macro added whenever called. Takes {args}.
{alias}		|	 $aliasName @ A 					|	Every time $aliasName is encountered, replace with register A (useful for programming formatting.)
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
	"nop": "0000", "set": "0001", "mov": "0010", "and": "0011",
	"or": "0100", "not": "0101", "lss": "0110", "equ": "0111",
	"gtr": "1000", "add": "1001", "sub": "1010", "mul": "1011",
	"div": "1100", "abs": "1101", "brn": "1110", "upd": "1111"
}

infixOperatorsList = {
	"+": "add", "-": "sub", "*": "mul", "/": "div", "%": "mod",
	"!": "not", "&": "and", "|": "or", "^": "xor",
	">": "gtr", "<": "lss", ">=": "gte", "<=": "lse", "==": "equ",
	"=": "set", "~": "mov",
}



class FabricationError(Exception):
	def __init__(self, error="Fabrication Failed!"):
		self.message = f"Fabrication Error; {error}"
		super().__init__(self.message)



def toBin(value, signed=False):
	if signed:
		if not (-128 <= value < 128):
			raise FabricationError(f"Immediate values must be signed 8-bit Integers [-128 → 127]: Encountered value of {value}")
	
		if value < 0:
			# Handle two's complement for negative values
			return format((1 << 8) + value, '08b')
		else:
			return format(value, '08b')
	else:
		if not (0 <= value < 256):
			raise FabricationError(f"Register indices must be unsigned 8-bit Integers [0 → 255]: Encountered value of {value}")

		return format(value, '08b')




def SET(register, value):
	return opcodes["set"] + register + toBin(int(value) + 128)



def BRN(line, condition, bits):
	binaryLine = str(bin(line)[2:]).zfill(16)
	halfA, halfB = int(binaryLine[:8], 2), int(binaryLine[8:], 2)
	return (
		"0011" + SET(toBin(10), halfA), #Set first JMP register to the first 8 bits.
		"0011" + SET(toBin(9), halfB), #Set second JMP register to the final 8 bits.
		"00" + bits[2] + "0" + opcodes["brn"] + toBin(condition) + toBin(0),
	)



def convertAllToBin(operator, convertedA, convertedB):
	width, height, blank = toBin(24), toBin(16), "0000"
	zero, one, rOP = toBin(0), toBin(1), toBin(15)

	initial4Bits = f"00{str(int(bool(convertedA[0])))}{str(int(bool(convertedB[0])))}"
	A, B = convertedA[1], convertedB[1]
	
	match operator:
		case "mov" | "and" | "or" | "lss" | "equ" | "gtr" | "add" | "sub" | "mul" | "div" | "rec" | "lne":
			return (
				initial4Bits + opcodes[operator] + toBin(A) + toBin(B),
			)

		case "set":
			return (
				initial4Bits + SET(toBin(A), B),
			)

		case "not" | "abs":
			return (
				initial4Bits + opcodes[operator] + toBin(A) + blank,
			)

		case "sgn":
			return (
				initial4Bits + opcodes["abs"] + toBin(A) + blank,
				initial4Bits[:3] + "0" + opcodes["div"] + toBin(A) + rOP,
			)

		case "inv":
			return (
				"000" + initial4Bits[3] + opcodes["sub"] + zero + toBin(A),
			)

		case "mod":
			return (
				initial4Bits + opcodes["div"] + toBin(A) + toBin(B),
				"000" + initial4Bits[3] + opcodes["mul"] + rOP + toBin(B),
				"000" + initial4Bits[3] + opcodes["sub"] + toBin(A) + rOP,
			)

		case "xor":
			return (
				initial4Bits + opcodes["equ"] + toBin(A) + toBin(B),
				blank + opcodes["not"] + rOP + blank,
			)

		case "gte":
			return (
				initial4Bits + opcodes["lss"] + toBin(A) + toBin(B),
				blank + opcodes["not"] + rOP + blank,
			)

		case "lse":
			return (
				opcodes["gtr"] + toBin(A) + toBin(B),
				blank + opcodes["not"] + rOP + blank,
			)

		case "jmp":
			return BRN(A, one, initial4Bits)

		case "brn":
			return BRN(A, B, initial4Bits)

		case "ext":
			return BRN(markers["_end"], one, initial4Bits)

		case "ramread":
			return (
				"000" + initial4Bits[3] + SET(toBin(14), A),
				"0000" + opcodes["mov"] + toBin(12) + rOP,
			)

		case "ramwrite":
			return (
				"000" + initial4Bits[3] + SET(toBin(14), A),
				"00" + initial4Bits[3] + "0" + opcodes["mov"] + toBin(B) + toBin(13),
				"0001" + SET(toBin(11), one),
			)

		case "col":
			raise FabricationError("Replace these with equivalent RAM chain instructions.")

		case "ptr":
			raise FabricationError("Replace these with equivalent RAM chain instructions.")

		case "pix":
			raise FabricationError("Replace these with equivalent RAM chain instructions.")

		case "clr":
			raise FabricationError("Replace these with equivalent RAM chain instructions.")

		case _:
			raise FabricationError(f"Unknown Command encountered: {operator} {a} {b}")



def convertValues(A):
	if A.startswith("r"):
		registerIndex = int(A.replace("r", ""))
		if registerIndex > 17: raise FabricationError(f"r{registerIndex} is out of range for registers.\nr0-r15 for values\nr16-r17 for true/false.")
		return False, registerIndex

	elif A.startswith("#"):
		return True, int(A.replace("#", ""))

	elif A.startswith(":"):
		return None, markers[A.replace(":", "")]

	else:
		return None, str(A) #Ensure the fallback is a string.



def convertLine(line):
	operands = line.split(" ")
	operands = [operand.strip() for operand in operands if operand != ""]
	if len(operands) == 2:
		operands.append("0")
	elif len(operands) == 1:
		operands.extend(["0", "0"])

	if operands[0] in opcodes or operands[0] in ("ext", "lse", "gte", "inv", "sgn", "jmp", "xor", "mod", "ramwrite", "ramread"):
		#Postfix
		operator, A, B = operands

	elif operands[1] in infixOperatorsList:
		#Infix
		A, operator, B = operands
		operator = infixOperatorsList[operator]

	elif operands[0] in ("!",):
		operator, A, B = operands
		operator = infixOperatorsList[operator]

	else:
		raise FabricationError(f"Unknown Command encountered: {operands}")

	convertedA = convertValues(A)
	convertedB = convertValues(B)

	instructionList = convertAllToBin(operator, convertedA, convertedB)
	hexList = [f"{int(instruction, 2):06X}" for instruction in instructionList]
	return hexList



def replaceMacros(lines, depth=0, activeMacros=None, previousMacro=None):
	if activeMacros is None:
		activeMacros = set()
	macrosReplaced = []
	lineNum = 0


	#Check for excessive recursion depth to prevent absurdly long chaining.
	maxRecursionDepth = 32
	if depth > maxRecursionDepth:
		raise FabricationError("Exceeded maximum macro recursion depth (32)")

	while lineNum < len(lines):
		curLine = lines[lineNum]

		#Macro has been defined.
		if curLine.startswith("def"):
			macroData = curLine.split(" ")
			if len(macroData) < 2:
				raise FabricationError(f"Macro definition missing name and/or parameters: {curLine}")

			macroName = macroData[1].replace("%", "")
			macroParams = macroData[2:]
			macroLines = []
			i = 0

			#Save the contents of the macro.
			while True:
				i += 1
				macroLine = lines[lineNum + i]
				if macroLine.startswith("end"):
					lineNum += i
					macros[macroName] = (macroLines, macroParams)
					break
				macroLines.append(macroLine)


		#Macro has been called.
		elif curLine.startswith("%"):
			lineData = curLine.split(" ")
			macroName = lineData[0][1:]

			#Check the macro was defined beforehand.
			if macroName not in macros:
				raise FabricationError(f"Macro %{macroName} is not defined.")

			#Prevent macros from making infinite recursive loops.
			if macroName in activeMacros:
				if previousMacro is None:
					errorMessage = f"Infinite Recursion; Attempted to unpack %{macroName}."
				elif previousMacro == macroName:
					errorMessage = f"Infinite Recursion; Attempted to unpack %{macroName} within itself"
				else:
					errorMessage = f"Infinite Recursion; Attempted to unpack %{macroName} within %{previousMacro}.\nThis resulted in a chain of macros, in a loop."

				raise FabricationError(errorMessage)


			#Begin expansion of macro
			macroLines, macroParams = macros[macroName]
			if len(lineData[1:]) != len(macroParams):
				raise FabricationError(f"Macro {macroName} expects {len(macroParams)} arguments, but got {len(lineData[1:])}")

			#Change macro args to their called counterparts.
			paramMapping = dict(zip(macroParams, lineData[1:]))
			activeMacros.add(macroName)  # Track macro to prevent re-expansion


			#Expand the macro to the line it was called at.
			expandedLines = []
			for macroLine in macroLines:
				processedLine = macroLine
				for param, arg in paramMapping.items():
					processedLine = processedLine.replace(param, arg)
				expandedLines.append(processedLine)


			#If a macro was found inside this macro, recursively unpack that too.
			macrosReplaced.extend(replaceMacros(expandedLines, depth + 1, activeMacros, macroName))
			activeMacros.remove(macroName)


		#Any other lines.
		else:
			macrosReplaced.append(curLine)

		lineNum += 1

	return macrosReplaced



def replaceAliases(lines):
	aliases = {
		"rop": "r15",
		"true": "r16",
		"false": "r17",
	}
	aliasReplaced = []

	for lineNum, curLine in enumerate(lines):
		"""
		Define aliasing for register names like so;
		 varName  @ r1
		Every time varName is written, it is replaced by r1 by the fabricator.
		Allows for nicer formatting of CFAB.
		"""
		operands = curLine.split(" ")
		if len(operands) == 3:
			if operands[1] == "@":
				aliases[operands[0].replace("$", "")] = operands[2]
				continue

		fixedLine = curLine
		for alias, reg in aliases.items():
			fixedLine = fixedLine.replace(f"${alias}", reg).replace(alias, reg)

		if fixedLine not in aliasReplaced:
			aliasReplaced.append(fixedLine)

	return aliasReplaced




if __name__ == "__main__":
	filename = "ramtest"
	with open(f"cfab\\{filename}.cfab", "r") as CFABFile:
		readlines = CFABFile.readlines()
		partial_lines = [line.strip().lower() for line in readlines if not line.startswith("//")]
		lines = [line for line in partial_lines if line != ""]


		macros, markers = {}, {}
		macrosReplaced = replaceMacros(lines)
		aliasReplaced = replaceAliases(macrosReplaced)
		del macrosReplaced


		for curLine in aliasReplaced:
			if curLine.startswith(":"):
				"""
				Process markers, which are written like so;
				 :marker
				You may jump back to these using BRN, JMP or EXT commands, like so;
				 JMP :marker
				EXT Always jumps to the final line of the instructions.
				"""
				markers[curLine.replace(":", "")] = [accLine for accLine in aliasReplaced if ((not (accLine.startswith(":") or accLine == "" or accLine.startswith("//"))) or accLine == curLine)].index(curLine)+1
		markers["_end"] = len(aliasReplaced)-1


		fabricated = []
		#print(aliasReplaced)
		for line in aliasReplaced:
			if not line.startswith(":"):
				#Convert lines using convertLine().
				fabricated.extend(convertLine(line))

		del macros, markers
		del aliasReplaced

		
		#Make the list of hex instructions into a set of bytes.
		combinedHex = ""
		for hexInstruction in fabricated:
			combinedHex += hexInstruction

		if len(combinedHex) % 2 == 1: combinedHex += "0"

		combinedBytes = bytes.fromhex(combinedHex)


	with open(f"data\\{filename}.dat", "wb") as outFile:
		#Write to a file.
		outFile.write(combinedBytes)