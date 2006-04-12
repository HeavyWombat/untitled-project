TITLE = project

TARGETDIR = ../..

VPATH  = ../../src
VPATH += :../../src/win
VPATH += :../../src/tinyxml
VPATH += :../../src/add

OBJECTS = gdv_os.o
OBJECTS += gdv_main.o
OBJECTS += camera.o
OBJECTS += geometry.o
OBJECTS += additionalMath.o
OBJECTS += gdv_resources.o
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

CFLAGS = -I.
CFLAGS += -I../../include/framework
CFLAGS += -I../../include
CFLAGS += -I../../src/tinyxml
CFLAGS += -I../../include/add
CFLAGS += -Wall -Werror -O3

LDFLAGS = -L.
LDFLAGS += -L../../lib/win
LDFLAGS += -lopengl32 -lglu32 -lmingw32 -mwindows -lfake -lz -lfmodex

CC = g++
RM = rm -f 
MV = mv

all: $(TITLE)

$(TITLE): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(MV) $(TITLE).exe $(TARGETDIR)

.PHONY: clean

clean:
	-$(RM) $(OBJECTS)
	

