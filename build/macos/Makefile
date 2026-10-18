TITLE = project

TARGETDIR = ../..

VPATH  = ../../src
VPATH += :../../src/macos
VPATH += :../../src/tinyxml
VPATH += :../../src/add

OBJECTS = gdv_os.o
OBJECTS += gdv_main.o
OBJECTS += camera.o
OBJECTS += geometry.o
OBJECTS += additionalMath.o
OBJECTS += gdv_resources.o
OBJECTS += image_loader.o
OBJECTS += display.o
OBJECTS += light.o
OBJECTS += objecthandler.o
OBJECTS += tinyxmlerror.o
OBJECTS += tinyxmlparser.o
OBJECTS += tinystr.o
OBJECTS += tinyxml.o
OBJECTS += cameraMovement.o
OBJECTS += landscape.o
OBJECTS += skybox.o
OBJECTS += objects.o
OBJECTS += trees.o
OBJECTS += selection.o
OBJECTS += targets.o
OBJECTS += sound.o
OBJECTS += gearup.o
OBJECTS += help.o
OBJECTS += text.o
OBJECTS += splash.o
OBJECTS += ttext.o
OBJECTS += hud.o
OBJECTS += rifle.o
OBJECTS += rain.o
OBJECTS += environment.o

CFLAGS = -I../../include/framework
CFLAGS += -I../../include
CFLAGS += -I../../src/tinyxml
CFLAGS += -I../../include/add
CFLAGS += -Wall -Wno-deprecated-declarations -O2
# Try pkg-config, fall back to Homebrew defaults if not available
CFLAGS += $(shell pkg-config --cflags libpng 2>/dev/null || echo "-I/usr/local/include -I/opt/homebrew/include")

LDFLAGS = -framework OpenGL
LDFLAGS += -framework Cocoa
LDFLAGS += -framework OpenAL
LDFLAGS += -lz
# Try pkg-config, fall back to Homebrew defaults if not available
LDFLAGS += $(shell pkg-config --libs libpng 2>/dev/null || echo "-L/usr/local/lib -L/opt/homebrew/lib -lpng")

CC = clang
CXX = clang++
RM = rm -f
MV = mv

all: $(TITLE)

# Compile Objective-C files
gdv_os.o: gdv_os.m
	$(CC) -c -o $@ $< $(CFLAGS)

# Compile C++ files
objecthandler.o: objecthandler.c
	$(CXX) -c -o $@ $< $(CFLAGS) -std=c++98

tinyxml.o: tinyxml.cpp
	$(CXX) -c -o $@ $< $(CFLAGS) -std=c++98

tinyxmlerror.o: tinyxmlerror.cpp
	$(CXX) -c -o $@ $< $(CFLAGS) -std=c++98

tinyxmlparser.o: tinyxmlparser.cpp
	$(CXX) -c -o $@ $< $(CFLAGS) -std=c++98

tinystr.o: tinystr.cpp
	$(CXX) -c -o $@ $< $(CFLAGS) -std=c++98

# Compile C files
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) -std=c99

$(TITLE): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)
	$(MV) $(TITLE) $(TARGETDIR)

.PHONY: clean
clean:
	-$(RM) $(OBJECTS)
	-$(RM) $(TARGETDIR)/$(TITLE)
