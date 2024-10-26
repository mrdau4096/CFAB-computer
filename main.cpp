#include <C:/Users/User/Documents/code/.cpp/glew-2.1.0/include/GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <C:/Users/User/Documents/code/.cpp/glm/glm.hpp>
#include <C:/Users/User/Documents/code/.cpp/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <vector>
#include <cstdint>
#include <chrono>
#include <thread>
using namespace std;
using namespace glm;


const int outputRegister = 255;
const int instructionLength = 5;
const int frameBufferWidth = 24;
const int frameBufferHeight = 16;
const int pixelSize = 25;
int8_t registers[256];


//Debug and other testing bools.
const bool printInstructionCalls = false;
const bool debugFrameBuffer = false;


class FrameBuffer {
public:
	FrameBuffer(int width, int height) : width(width), height(height) {
		data.resize(width * height * 3); // 3 channels for RGB
		for (int i = 0; i < width * height * 3; i += 3) {
            data[i] = 255;     // Red component
            data[i + 1] = 255; // Green component
            data[i + 2] = 255; // Blue component
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
            unsigned char r = data[index];     // Red component
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


int main() {
	memset(registers, 0, sizeof(registers));

	int instructionNum = 0;
	string instructionsFileName, instructionsFull;
	string extension = ".dat";

	//Get the instructions from the file
	cout << "File name to use [NO EXTENSION]" << endl << "> ";
	cin >> instructionsFileName;
	//instructionsFileName = "testgraphics";
	ifstream dataFile(instructionsFileName + extension);
	if (dataFile.is_open()) {
		dataFile >> instructionsFull;
	} else {
		raise("Failed to load file " + instructionsFileName + extension);
		return -1;
	}
	int maxInstructions = instructionsFull.length() / instructionLength;


	//Create GLFW window
	if (!glfwInit()) {
		raise("Failed to initialize GLFW");
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(frameBufferWidth * pixelSize, frameBufferHeight * pixelSize, "FrameBuffer", NULL, NULL);
	if (!window) {
		glfwTerminate();
		raise("Failed to create GLFW window");
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glewExperimental = GL_TRUE; 
	if (glewInit() != GLEW_OK) {
		raise("Failed to initialize GLEW");
		return -1;
	}

	// Set up orthographic projection for 2D
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0); // Set the orthographic projection

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();




	GLuint textureID = createTexture();
	string currentInstruction;
	int function, ptX, ptY, colour;
	int xDist, yDist, xSign, ySign, err;
	int xCoord, yCoord;
	uint8_t A, B, result;
	bool registerManipulationFunction, updateOutput;

	//Update Screen
	glClear(GL_COLOR_BUFFER_BIT);
	//Render framebuffer to screen.
	updateTexture(textureID);
	renderQuad(textureID); // Render the textured quad
	glfwSwapBuffers(window);
	glfwPollEvents();
	while ((instructionNum < maxInstructions) and (!glfwWindowShouldClose(window))) {
		//Main loop for running instructions



		int instructionStart = instructionNum * instructionLength;
		currentInstruction = instructionsFull.substr(instructionStart, instructionLength);
		function = stoi(currentInstruction.substr(0, 1), 0, 16);
		A = stoi(currentInstruction.substr(1, 2), 0, 16);
		B = stoi(currentInstruction.substr(3, 2), 0, 16);
		registerManipulationFunction = function <= 1;


		//For logical functions, 0 is false and any other value is true.
		result = 0;
		updateOutput = true;
		switch (function){
			case 0:		//SET - Sets reg A to value B
				if (printInstructionCalls) {print(string("SET") + " " + to_string(A) + " " + to_string(B - 128));}
				registers[A] = static_cast<int8_t>(B) - 128;  // Treat B as signed
				updateOutput = false;
				break;


			case 1:		//MOV - Moves contents of reg A to reg B
				if (printInstructionCalls) {print(string("MOV") + " " + to_string(A) + " " + to_string(B));}
				registers[B] = registers[A];
				registers[A] = 0;
				updateOutput = false;
				break;


			case 2:		//AND - Logical AND of A and B
				if (printInstructionCalls) {print(string("AND") + " " + to_string(A) + " " + to_string(B));}
				result = registers[A] & registers[B];
				break;


			case 3:		//OR  - Logical OR of A and B
				if (printInstructionCalls) {print(string("OR ") + " " + to_string(A) + " " + to_string(B));}
				result = registers[A] || registers[B];
				break;


			case 4:		//NOT - Logical NOT of A {B unused}
				if (printInstructionCalls) {print(string("NOT") + " " + to_string(A) + " " + to_string(B));}
				result = !registers[A];
				break;


			case 5:		//LSS - If A is less than B
				if (printInstructionCalls) {print(string("LSS") + " " + to_string(A) + " " + to_string(B));}
				result = registers[A] < registers[B];
				break;


			case 6:		//EQU - If A is equal to B
				if (printInstructionCalls) {print(string("EQU") + " " + to_string(A) + " " + to_string(B));}
				result = registers[A] == registers[B];
				break;


			case 7:		//GTR - If A is greater than B
				if (printInstructionCalls) {print(string("GTR") + " " + to_string(A) + " " + to_string(B));}
				result = registers[A] > registers[B];
				break;


			case 8:		//ADD - Mathmatical A add B
				if (printInstructionCalls) {print(string("ADD") + " " + to_string(A) + " " + to_string(B));}
				result = registers[A] + registers[B];
				break;


			case 9:		//SUB - Mathmatical A subtract B
				if (printInstructionCalls) {print(string("SUB") + " " + to_string(A) + " " + to_string(B));}
				result = registers[A] - registers[B];
				break;


			case 10:	//MUL - Mathmatical A multiplied by B
				if (printInstructionCalls) {print(string("MUL") + " " + to_string(A) + " " + to_string(B));}
				result = registers[A] * registers[B];
				break;


			case 11:	//DIV - Mathmatical A divided by B {Rounds DOWN}
				if (printInstructionCalls) {print(string("DIV") + " " + to_string(A) + " " + to_string(B));}
				if (registers[B] != 0) {
					result = floor(registers[A] / registers[B]);
				}
				break;


			case 12:	//ABS - Mathmatical absolute value of A {B unused}
				if (printInstructionCalls) {print(string("ABS") + " " + to_string(A) + " " + to_string(B));}
				result = abs(registers[A]);
				break;


			case 13:	//BRN - Jumps to instruction A if B is true.
				if (printInstructionCalls) {print(string("BRN") + " " + to_string(A) + " " + to_string(B));}
				if (registers[B] != 0) {
					// A is treated as an unsigned 8-bit integer for BRN
					instructionNum = static_cast<uint8_t>(A) - 1;  // Adjust for the loop increment
				}
				updateOutput = false;
				break;


			case 14:	//REC - Draws rectangle to the frameBuffer
				if (printInstructionCalls) {print(string("REC") + " " + to_string(A) + " " + to_string(B));}
				ptX = registers[252];
				ptY = registers[253];

				for (yCoord=0; yCoord < ptY; yCoord++) {
					for (xCoord=0; xCoord < ptX; xCoord++) {
						frameBuffer = updatePixel(xCoord + registers[A], yCoord + registers[B], frameBuffer);
					}
				}
				updateOutput = false;

				//Update Screen
				glClear(GL_COLOR_BUFFER_BIT);
				//Render framebuffer to screen.
				updateTexture(textureID);
				renderQuad(textureID); // Render the textured quad
				glfwSwapBuffers(window);
				glfwPollEvents();
				break;


			case 15:	//LNE - Draws line between 2 points (DIM & inputs used)
				if (printInstructionCalls) {print(string("LNE") + " " + to_string(A) + " " + to_string(B));}
				ptX = registers[252];
				ptY = registers[253];
				xCoord = registers[A];
				yCoord = registers[B];

				xDist = abs(xCoord - ptX);
				yDist = abs(yCoord - ptY);

				xSign = (ptX < xCoord) ? -1 : 1;
				ySign = (ptY < yCoord) ? -1 : 1;
			    
			    err = xDist - yDist;

			    while (true) {
				    frameBuffer = updatePixel(xCoord, yCoord, frameBuffer);

				    if (xCoord == ptX && yCoord == ptY) {
				        break;
				    }

				    int err2 = 2 * err;  // Temporary variable for error adjustment

				    // Adjust error term and coordinate based on Bresenham's condition
				    if (err2 > -yDist) {
				        err -= yDist;
				        xCoord += xSign;
				    }
				    if (err2 < xDist) {
				        err += xDist;
				        yCoord += ySign;
				    }
				}
			    updateOutput = false;

			    //Update Screen
				glClear(GL_COLOR_BUFFER_BIT);
				//Render framebuffer to screen.
				updateTexture(textureID);
				renderQuad(textureID); // Render the textured quad
				glfwSwapBuffers(window);
				glfwPollEvents();
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