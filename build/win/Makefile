TITLE = fussball
TARGETDIR = ../..
VPATH = ../../src:../../src/win
OBJECTS = gdv_os.o gdv_main.o camera.o geometry.o skybox.o gdv_resources.o display.o stadium.o light.o landscape.o

CFLAGS = -I. -I../../src -I../../include -Wall -Werror
LDFLAGS = -L. -L../../lib/win -lopengl32 -lglu32 -lmingw32 -mwindows -lfake -lz
CC = gcc
RM = rm -f 
MV = mv

all: $(TITLE)

$(TITLE): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(MV) $(TITLE).exe $(TARGETDIR)

.PHONY: clean

clean:
	-$(RM) $(OBJECTS)
	

