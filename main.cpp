#include <C:/Users/User/Documents/code/.cpp/glew-2.1.0/include/GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <C:/Users/User/Documents/code/.cpp/glm/glm.hpp>
#include <C:/Users/User/Documents/code/.cpp/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include <cstdint>
#include <bitset>
using namespace std;
using namespace glm;


const int outputRegister = 31;
const int instructionLength = 5;
const int frameBufferWidth = 24;
const int frameBufferHeight = 16;
const int pixelSize = 25;

//16 Registers, and 256 RAM Addresses.
int8_t registers[32];
int8_t randomAccessMemory[512];


//Debug and other testing bools.
const bool printInstructionCalls = false;
const bool debugFrameBuffer = false;


class FrameBuffer {
public:
	FrameBuffer(int width, int height) : width(width), height(height) {
		data.resize(width * height * 3); // 3 channels for RGB
		for (int i = 0; i < width * height * 3; i += 3) {
			data[i] = 0;	 // Red component
			data[i + 1] = 0; // Green component
			data[i + 2] = 0; // Blue component
		}
	}

	// Overload the operator for 2D access
	unsigned char* operator[](int y) {
		return &data[y * width * 3]; // Return a pointer to the start of the row
	}

	// Get the width and height of the framebuffer
	int getWidth() const { return width; }
	int getHeight() const { return height; }

	unsigned char* getData() {
		return data.data(); // Return a pointer to the raw data
	}

private:
	int width, height;
	vector<unsigned char> data; // Stores pixel data
};

FrameBuffer frameBuffer(frameBufferWidth, frameBufferHeight);

//Define colours;
vector<vector<unsigned char>> colourPalette = {
	{0x00, 0x00, 0x00}, // 0: Black
	{0x80, 0x00, 0x00}, // 1: Dark Red
	{0x00, 0x80, 0x00}, // 2: Dark Green
	{0x80, 0x80, 0x00}, // 3: Dark Yellow
	{0x00, 0x00, 0x80}, // 4: Dark Blue
	{0x80, 0x00, 0x80}, // 5: Dark Magenta
	{0x00, 0x80, 0x80}, // 6: Dark Cyan
	{0x80, 0x80, 0x80}, // 7: Dark Grey
	{0xBB, 0xBB, 0xBB}, // 8: Light Grey
	{0xBB, 0x00, 0x00}, // 9: Red
	{0x00, 0xBB, 0x00}, // A: Green
	{0xBB, 0xBB, 0x00}, // B: Yellow
	{0x00, 0x00, 0xBB}, // C: Blue
	{0xBB, 0x00, 0xBB}, // D: Magenta
	{0x00, 0xBB, 0xBB}, // E: Cyan
	{0xFF, 0xFF, 0xFF}  // F: White
};



void print(string str) {
	std::cout << str << endl;
}

void printFramebuffer(FrameBuffer frameBuffer) {
	int width = frameBuffer.getWidth();
	int height = frameBuffer.getHeight();
	unsigned char* data = frameBuffer.getData();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (y * width + x) * 3;
			unsigned char r = data[index];	 // Red component
			unsigned char g = data[index + 1]; // Green component
			unsigned char b = data[index + 2]; // Blue component
			
			// Print each pixel in the format (R, G, B)
			cout << "(" 
					  << hex << static_cast<int>(r) << "," 
					  << hex << static_cast<int>(g) << "," 
					  << hex << static_cast<int>(b) << ") ";
		}
		cout << endl;
	}
}


FrameBuffer updatePixel(int xCoord, int yCoord, FrameBuffer frameBuffer) {
	int colourIndex = std::min(std::max(registers[254] + 0, 0), static_cast<int>(colourPalette.size()) - 1);
	int frameBufferWidth = frameBuffer.getWidth();
	int frameBufferHeight = frameBuffer.getHeight();
	//Out of range errors. Just ignores these pixels ("draws them off screen")
	if (xCoord < 0 || xCoord >= frameBufferWidth) {
		return frameBuffer;
	} else if (yCoord < 0 || yCoord >= frameBufferHeight) {
		return frameBuffer;
	}

	vector<unsigned char> currentColour = colourPalette[colourIndex];
	int index = ((yCoord * frameBufferWidth) + xCoord) * 3;
	int rIndex = index + 0;
	int gIndex = index + 1;
	int bIndex = index + 2;
	frameBuffer.getData()[rIndex] = currentColour[0]; // Red component
	frameBuffer.getData()[gIndex] = currentColour[1]; // Green component
	frameBuffer.getData()[bIndex] = currentColour[2]; // Blue component
	return frameBuffer;
}
void raise(string err) {
	std::cerr << err << endl;
	string end;
	cin >> end;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

GLuint createTexture() {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

	return textureID;
}

void updateTexture(GLuint textureID) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	// Set texture parameters (wrapping and filtering)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Upload the framebuffer pixel data (8x8 RGB values)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameBufferWidth, frameBufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, frameBuffer.getData());

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
}

void renderQuad(GLuint textureID) {
	// Enable texturing
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Begin drawing the quad
	glBegin(GL_QUADS);
	
	// Specify vertices and texture coordinates
	glTexCoord2f( 0.0f,  0.0f); glVertex2f( 0.0f,  1.0f); // Top-left
	glTexCoord2f( 1.0f,  0.0f); glVertex2f( 1.0f,  1.0f); // Top-right
	glTexCoord2f( 1.0f,  1.0f); glVertex2f( 1.0f,  0.0f); // Bottom-right
	glTexCoord2f( 0.0f,  1.0f); glVertex2f( 0.0f,  0.0f); // Bottom-left

	glEnd();

	glDisable(GL_TEXTURE_2D); // Disable texturing
}

string toHex(int dec) {
	stringstream ss;
	ss << std::hex << dec; // int decimal_value
	string res ( ss.str() );
	if (res.length() == 1) {
		return string("0") + res;
	}
	return res;
}


GLFWwindow* initializeWindow(int width, int height, const char* title) {
	if (!glfwInit()) {
		raise("Failed to initialize GLFW");
		return nullptr;
	}
	GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window) {
		glfwTerminate();
		raise("Failed to create GLFW window");
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	return window;
}


bool loadInstructions(const string& filename, vector<uint8_t>& instructionData) {
	ifstream dataFile(filename, ios::binary | ios::ate);
	if (!dataFile) {
		raise("Failed to open file: " + filename);
		return false;
	}
	streamsize fileSize = dataFile.tellg();
	dataFile.seekg(0, ios::beg);
	instructionData.resize(fileSize);

	if (!dataFile.read(reinterpret_cast<char*>(instructionData.data()), fileSize)) {
		raise("Failed to read file: " + filename);
		return false;
	}
	return true;
}


int main() {
	//Initialise any values that need to be in scope;
	memset(registers, 0, sizeof(registers));

	//General Variables
	unsigned long instructionNum = 0;
	string instructionsFileName, extension = ".dat", folder = "data\\";
	vector<uint8_t> instructionData;
	
	//Instruction related variables;
	int8_t result, byte1, byte2, byte3;
	uint8_t combinedHalfLower, combinedHalfUpper;
	uint16_t combinedRegisters;
	bool registerManipulationFunction, updateOutput;



	//Load instructions from data file;
	cout << "File name to use [NO EXTENSION]" << endl << "> ";
	cin >> instructionsFileName;
	string fullFilePath = folder + instructionsFileName + extension;
	if (!loadInstructions(fullFilePath, instructionData)) {
		return -1;
	}
	int maxInstructions = instructionData.size() / 3;




	//Create GLFW window to show contents of frameBuffer;
	GLFWwindow* window = initializeWindow(frameBufferWidth * pixelSize, frameBufferHeight * pixelSize, "FrameBuffer");
	if (!window) return -1;

	//Initialize GLEW;
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		raise("Failed to initialize GLEW");
		return -1;
	}

	//Set up orthographic projection matrix;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Initialise Texture;
	GLuint textureID = createTexture();



	//Initial screen texture;
	glClear(GL_COLOR_BUFFER_BIT);
	updateTexture(textureID);
	renderQuad(textureID);
	glfwSwapBuffers(window);
	glfwPollEvents();



	//Main instruction loop;
	//(Iterates through instructions in instructionData.)
	while ((instructionNum < maxInstructions) && !glfwWindowShouldClose(window)) {
		int baseIndex = instructionNum * 3;

		//Read Bytes from file.
		byte1 = instructionData[baseIndex];
		byte2 = instructionData[baseIndex + 1];
		byte3 = instructionData[baseIndex + 2];
		bool AImmediate =  (byte1 & 0b00100000);
		bool BImmediate =  (byte1 & 0b00010000);
		uint8_t function = (byte1 & 0b00001111);
		uint8_t operandA = byte2;
		uint8_t operandB = byte3;

		int8_t *regA = &registers[operandA];
		int8_t *regB = &registers[operandB];

		int8_t A = (AImmediate) ? static_cast<int8_t>(operandA) - 128 : registers[operandA];
		int8_t B = (BImmediate) ? static_cast<int8_t>(operandB) - 128 : registers[operandB];



		//For logical functions, 0 is false and any other value is true.
		result = 0;
		updateOutput = true;
		switch (function){
			case 0:		//NOP - Do nothing, blank opcode.
				updateOutput = false;
				break;


			case 1:		//SET - Sets reg A to value B
				if (printInstructionCalls) {print(string("SET") + " r" + to_string(operandA) + " to the value: " + to_string(B));}
				updateOutput = false;
				if (AImmediate) {
					//Do not accept this; You cannot set an immediate value to another.
					//You ARE allowed to SET 1 register to another; This copys the data. MOV moves the data.
					break;
				}

				*regA = B;


				//Treat the 8 bit values in registers 29 and 30 as 1 16 bit value.
				combinedHalfUpper = registers[29];
				combinedHalfLower = registers[30];

				combinedRegisters = bitset<8>((combinedHalfUpper << 8) | combinedHalfLower).to_ulong();


				//Only the SET and MOV commands can affect RAM, so check here.
				//Set the RAM Address to contain the input data if the write register is true.
				if (registers[26]) {
					registers[26] = 0;
					randomAccessMemory[combinedRegisters] = registers[28];
				}

				//Set the READ Register to the contents of the current RAM address.
				registers[27] = randomAccessMemory[combinedRegisters];

				break;


			case 2:		//MOV - Moves contents of reg A to reg B
				if (printInstructionCalls) {print(string("MOV") + ", contents of r" + to_string(operandA) + " to r" + to_string(operandB));}
				if (AImmediate || BImmediate) {
					//You cannot MOV immediate values to each other. Use SET.
					break;
				}

				*regB = *regA;
				*regA = 0;
				updateOutput = false;


				//Treat the 8 bit values in registers 29 and 30 as 1 16 bit value.
				combinedHalfUpper = registers[29];
				combinedHalfLower = registers[30];

				combinedRegisters = bitset<8>((combinedHalfUpper << 8) | combinedHalfLower).to_ulong();


				//Only the SET and MOV commands can affect RAM, so check here.
				//Set the RAM Address to contain the input data if the write register is true.
				if (registers[26]) {
					registers[26] = 0;
					randomAccessMemory[combinedRegisters] = registers[28];
				}

				//Set the READ Register to the contents of the current RAM address.
				registers[27] = randomAccessMemory[combinedRegisters];

				break;


			case 3:		//AND - Logical AND of A and B
				if (printInstructionCalls) {print(string("AND") + ", values; " + to_string(A) + " & " + to_string(B));}
				result = A & B;
				break;


			case 4:		//OR  - Logical OR of A and B
				if (printInstructionCalls) {print(string("OR ") + ", values; " + to_string(A) + " | " + to_string(B));}
				result = A || B;
				break;


			case 5:		//NOT - Logical NOT of A {B unused}
				if (printInstructionCalls) {print(string("NOT") + ", !" + to_string(A));}
				result = !A;
				break;


			case 6:		//LSS - If A is less than B
				if (printInstructionCalls) {print(string("LSS") + ", values; " + to_string(A) + " < " + to_string(B));}
				result = A < B;
				break;


			case 7:		//EQU - If A is equal to B
				if (printInstructionCalls) {print(string("EQU") + ", values; " + to_string(A) + " == " + to_string(B));}
				result = A == B;
				break;


			case 8:		//GTR - If A is greater than B
				if (printInstructionCalls) {print(string("GTR") + ", values; " + to_string(A) + " > " + to_string(B));}
				result = A > B;
				break;


			case 9:		//ADD - Mathmatical A add B
				if (printInstructionCalls) {print(string("ADD") + ", values; " + to_string(A) + " + " + to_string(B));}
				result = A + B;
				break;


			case 10:	//SUB - Mathmatical A subtract B
				if (printInstructionCalls) {print(string("SUB") + ", values; " + to_string(A) + " - " + to_string(B));}
				result = A - B;
				break;


			case 11:	//MUL - Mathmatical A multiplied by B
				if (printInstructionCalls) {print(string("MUL") + ", values; " + to_string(A) + " * " + to_string(B));}
				result = A * B;
				break;


			case 12:	//DIV - Mathmatical A divided by B {Rounds DOWN}
				if (printInstructionCalls) {print(string("DIV") + ", values; " + to_string(A) + " / " + to_string(B));}
				if (B != 0) {
					result = floor(A / B);
				}
				break;


			case 13:	//ABS - Mathmatical absolute value of A {B unused}
				if (printInstructionCalls) {print(string("ABS") + ", abs(" + to_string(A) + ")");}
				result = abs(A);
				break;


			case 14:	//BRN - Jumps to instruction A if B is true.

				//Treat the 8 bit values in registers 24 and 25 as 1 16 bit value.
				combinedHalfUpper = registers[24] + 128;
				combinedHalfLower = registers[25] + 128;

				combinedRegisters = bitset<8>((combinedHalfUpper << 8) | combinedHalfLower).to_ulong();
				
				if (printInstructionCalls) {print(string("BRN") + " to line "+ to_string(combinedRegisters) + " if " + to_string(A) + " == 1");}

				if (A != 0) {
					// A is treated as an unsigned 8-bit integer for BRN
					instructionNum = combinedRegisters;  // Adjust for the loop increment
				}
				updateOutput = false;
				break;


			case 15:	//UPD - Updates the screen shown to the user.
				if (printInstructionCalls) {print("UPD(ate) the screen.");}
				glClear(GL_COLOR_BUFFER_BIT);
				//Render framebuffer to screen.
				updateTexture(textureID);
				renderQuad(textureID); // Render the textured quad
				glfwSwapBuffers(window);
				glfwPollEvents();
				updateOutput = false;
				break;


			default:
				break;
		}

		if (result >= 128) {
			result = 127;
		} else if (result < -128) {
			result = -128;
		}

		if (updateOutput) {registers[outputRegister] = result;}

		instructionNum++; 
	}


	if (debugFrameBuffer) {printFramebuffer(frameBuffer);}



	print("--Reached end of instructions--");


	while (!glfwWindowShouldClose(window)) {
		//this_thread::sleep_for(33ms);

		glClear(GL_COLOR_BUFFER_BIT);
		//Render framebuffer to screen.
		renderQuad(textureID); // Render the textured quad
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}