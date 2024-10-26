CC = g++
CFLAGS = -I"C:/Users/User/Documents/code/.cpp/glew-2.1.0/include" \
          -I"C:/Users/User/Documents/code/.cpp/glm" \
          -I"C:/Users/User/Documents/code/.cpp/glfw-3.4.bin.WIN64/include"
LIBS = -L"C:/Users/User/Documents/code/.cpp/glew-2.1.0/lib/Release/x64" \
       -L"C:/Users/User/Documents/code/.cpp/glfw-3.4.bin.WIN64/lib-mingw-w64" \
       -lglew32 -lglfw3 -lopengl32 -lglu32 -luser32 -lgdi32
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: app

app: $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o app

%.o: %.cpp
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	del $(OBJECTS) app.exe
