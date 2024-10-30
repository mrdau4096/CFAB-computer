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


const int outputRegister = 15;
const int instructionLength = 5;
const int frameBufferWidth = 24;
const int frameBufferHeight = 16;
const int pixelSize = 25;

//16 Registers, and 256 RAM Addresses.
int8_t registers[16];
int8_t randomAccessMemory[256];


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
	int function, ptX, ptY, colour;
	int xDist, yDist, xSign, ySign, err;
	int xCoord, yCoord;
	uint8_t A, B, result, byte1, byte2, byte3, jumpHalfLower, jumpHalfUpper;
	uint16_t binJumpLine;
	bool registerManipulationFunction, updateOutput;



	//Load instructions from data file;
	cout << "File name to use [NO EXTENSION]" << endl << "> ";
	cin >> instructionsFileName;
	string fullFilePath = folder + instructionsFileName + extension;
	if (!loadInstructions(fullFilePath, instructionData)) {
		return -1;
	}
	int maxInstructions = instructionData.size() / 2.5;




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
		int offset = floor(instructionNum / 2);
		int baseIndex = instructionNum * 2 + offset;

		if (instructionNum % 2 == 0) { // Even-index instructions
			byte1 = instructionData[baseIndex];
			byte2 = instructionData[baseIndex + 1];
			byte3 = instructionData[baseIndex + 2];
			function = byte1 >> 4;
			A = ((byte1 & 0b00001111) << 4) | (byte2 >> 4);
			B = ((byte2 & 0b00001111) << 4) | (byte3 >> 4);
		} else { // Odd-index instructions
			byte1 = instructionData[baseIndex];
			byte2 = instructionData[baseIndex + 1];
			byte3 = instructionData[baseIndex + 2];
			function = (byte1 & 0b00001111);
			A = byte2;
			B = byte3;
		}



		if (function > 2) {
			//For all non-immediate values, 16 is True and 17 is False.
			A = (A > 15) ? ((A > 16) ? 0 : 1) : A;
			B = (B > 15) ? ((B > 16) ? 0 : 1) : B;
		}

		//For logical functions, 0 is false and any other value is true.
		result = 0;
		updateOutput = true;
		switch (function){
			case 0:		//NOP - Do nothing, blank opcode.
				updateOutput = false;
				break;


			case 1:		//SET - Sets reg A to value B
				if (printInstructionCalls) {print(string("SET") + " r" + to_string(A) + " to the value: " + to_string(B - 128));}
				updateOutput = false;
				if (A > 15) {
					//Do not accept this; it is simply a true/false immediate value not a register.
					break;
				}

				registers[A] = static_cast<int8_t>(B) - 128;  // Treat B as signed


				//Only the SET and MOV commands can affect RAM, so check here.
				//Set the RAM Address to contain the input data if the write register is true.
				if (registers[11]) {
					registers[11] = 0;
					//Is RAM index correct?
					randomAccessMemory[registers[14] + 128] = registers[13];
				}

				//Set the READ Register to the contents of the current RAM address.
				registers[12] = randomAccessMemory[registers[14] + 128];

				break;


			case 2:		//MOV - Moves contents of reg A to reg B
				if (printInstructionCalls) {print(string("MOV") + " contents of r" + to_string(A) + " to r" + to_string(B));}
				if (A > 15 and B < 16) {
					//True/False values
					registers[B] = (A > 16) ? 0 : 1;
				} else if (B > 15) {
					//Do not attempt to write to a pseudo-register.
					break;
				} else {
					registers[B] = registers[A];
					registers[A] = 0;
				}
				updateOutput = false;


				//Only the SET and MOV commands can affect RAM, so check here.
				//Set the RAM Address to contain the input data if the write register is true.
				if (registers[11]) {
					registers[11] = 0;
					randomAccessMemory[registers[14]] = registers[13];
				}

				//Set the READ Register to the contents of the current RAM address.
				registers[12] = randomAccessMemory[registers[14]];

				break;


			case 3:		//AND - Logical AND of A and B
				if (printInstructionCalls) {print(string("AND") + " r" + to_string(A) + " r" + to_string(B));}
				result = registers[A] & registers[B];
				break;


			case 4:		//OR  - Logical OR of A and B
				if (printInstructionCalls) {print(string("OR ") + " r" + to_string(A) + " r" + to_string(B));}
				result = registers[A] || registers[B];
				break;


			case 5:		//NOT - Logical NOT of A {B unused}
				if (printInstructionCalls) {print(string("NOT") + " r" + to_string(A));}
				result = !registers[A];
				break;


			case 6:		//LSS - If A is less than B
				if (printInstructionCalls) {print(string("LSS") + " r" + to_string(A) + " r" + to_string(B));}
				result = registers[A] < registers[B];
				break;


			case 7:		//EQU - If A is equal to B
				if (printInstructionCalls) {print(string("EQU") + " r" + to_string(A) + " r" + to_string(B));}
				result = registers[A] == registers[B];
				break;


			case 8:		//GTR - If A is greater than B
				if (printInstructionCalls) {print(string("GTR") + " r" + to_string(A) + " r" + to_string(B));}
				result = registers[A] > registers[B];
				break;


			case 9:		//ADD - Mathmatical A add B
				if (printInstructionCalls) {print(string("ADD") + " r" + to_string(A) + " r" + to_string(B));}
				result = registers[A] + registers[B];
				break;


			case 10:		//SUB - Mathmatical A subtract B
				if (printInstructionCalls) {print(string("SUB") + " r" + to_string(A) + " r" + to_string(B));}
				result = registers[A] - registers[B];
				break;


			case 11:	//MUL - Mathmatical A multiplied by B
				if (printInstructionCalls) {print(string("MUL") + " r" + to_string(A) + " r" + to_string(B));}
				result = registers[A] * registers[B];
				break;


			case 12:	//DIV - Mathmatical A divided by B {Rounds DOWN}
				if (printInstructionCalls) {print(string("DIV") + " r" + to_string(A) + " r" + to_string(B));}
				if (registers[B] != 0) {
					result = floor(registers[A] / registers[B]);
				}
				break;


			case 13:	//ABS - Mathmatical absolute value of A {B unused}
				if (printInstructionCalls) {print(string("ABS") + " r" + to_string(A));}
				result = abs(registers[A]);
				break;


			case 14:	//BRN - Jumps to instruction A if B is true.
				if (printInstructionCalls) {print(string("BRN") + " if r" + to_string(A));}

				jumpHalfUpper = randomAccessMemory[254] + 128;
				jumpHalfLower = randomAccessMemory[255] + 128;

				binJumpLine = (jumpHalfUpper << 8) | jumpHalfLower;

				if (registers[A] != 0) {
					// A is treated as an unsigned 8-bit integer for BRN
					instructionNum = bitset<8>(binJumpLine).to_ulong();  // Adjust for the loop increment
				}
				updateOutput = false;
				break;
				break;


			case 15:	//UPD - Updates the screen shown to the user.
				if (printInstructionCalls) {print("UPD the screen.");}
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
	print(to_string(randomAccessMemory[0]));

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