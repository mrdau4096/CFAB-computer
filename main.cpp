#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cstdint>
using namespace std;

const int outputRegister = 255;
const int instructionLength = 5;
int8_t registers[256];

int print(string str) {
	std::cout << str << endl;
	return 0;
}


int main() {
	int instructionNum = 0;
	string instructionsFileName, instructionsFull;
	string extension = ".dat";

	//Get the instructions from the file
	cout << "File name to use [NO EXTENSION]" << endl << "> ";
	cin >> instructionsFileName;
	//instructionsFileName = "testloop";
	ifstream dataFile(instructionsFileName + extension);
	if (dataFile.is_open()) {
		dataFile >> instructionsFull;
	} else {
		return 0;
	}
	

	int maxInstructions = instructionsFull.length() / instructionLength;
	string currentInstruction;
	int function;
	uint8_t A, B, result;
	bool registerManipulationFunction;
	while (instructionNum < maxInstructions) {
		//Main loop for running instructions
		int instructionStart = instructionNum * instructionLength;
		currentInstruction = instructionsFull.substr(instructionStart, instructionLength);
		function = stoi(currentInstruction.substr(0, 1), 0, 16);
		A = stoi(currentInstruction.substr(1, 2), 0, 16);
		B = stoi(currentInstruction.substr(3, 2), 0, 16);
		registerManipulationFunction = function <= 1;


		//For logical functions, 0 is false and any other value is true.
		result = 0;
		switch (function){
			case 0:		//SET - Sets reg A to value B
                registers[A] = static_cast<int8_t>(B);  // Treat B as signed
                break;

			case 1:		//MOV - Moves contents of reg A to reg B
				registers[B] = registers[A];
				registers[A] = 0;
				break;

			case 2:		//AND - Logical AND of A and B
				result = registers[A] & registers[B];
				break;

			case 3:		//OR  - Logical OR of A and B
				result = registers[A] || registers[B];
				break;

			case 4:		//NOT - Logical NOT of A {B unused}
				result = !registers[A];
				break;

			case 5:		//XOR - Logical XOR of A and B
				result = registers[A] != registers[B];
				break;

			case 6:		//EQU - If A is equal to B
				result = registers[A] == registers[B];
				break;

			case 7:		//GTR - If A is greater than B
				result = registers[A] > registers[B];
				break;

			case 8:		//LSS - If A is less than B
				result = registers[A] < registers[B];
				break;

			case 9:		//ADD - Mathmatical A add B
				result = registers[A] + registers[B];
				break;

			case 10:	//SUB - Mathmatical A subtract B
				result = registers[A] - registers[B];
				break;

			case 11:	//MUL - Mathmatical A multiplied by B
				result = registers[A] * registers[B];
				break;

			case 12:	//DIV - Mathmatical A divided by B {Rounds DOWN}
				if (B != 0) {
					result = registers[A] / registers[B];
				}
				break;

			case 13:	//MOD - Mathmatical A Modulo B {remainder}
				if (B != 0){
					result = registers[A] % registers[B];
				}
				break;

			case 14:	//ABS - Mathmatical absolute value of A {B unused}
				result = abs(registers[A]);
				break;

			case 15:	//BRN - Jumps to instruction A if B is true.
                if (registers[B] != 0) {
                    // A is treated as an unsigned 8-bit integer for BRN
                    instructionNum = static_cast<uint8_t>(A) - 1;  // Adjust for the loop increment
                }
				break;

			default:
				break;
		}

		if (result >= 128) {
			result = 127;
		} else if (result < -128) {
			result = -128;
		}

		registers[outputRegister] = result;


		instructionNum++;
	}
	print("end:"+to_string(registers[0]));
	string end;
	cin >> end;
}