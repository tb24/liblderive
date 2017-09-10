
uname := $(shell sh -c 'uname -s 2>/dev/null')

ifeq ($(uname),Darwin)
  GL_DIR = "OpenGL"
  LIBS=-lm -llsys -lglfw -framework Cocoa -framework OpenGL
endif

ifeq ($(uname),Linux)
  GL_DIR = "GL"
  LIBS=-llsys -lm -lglfw -lGL
endif

DEFINES=-DGL_DIR=$(GL_DIR) 

CFLAGS = -Wall -std=c99 -D_GNU_SOURCE -O3 $(DEFINES)

ifeq ($(mode),debug)
  CFLAGS = -Wall -std=c99 -D_GNU_SOURCE -O0 -g $(DEFINES)
endif

ifeq ($(mode),prof)
  CFLAGS = -Wall -std=c99 -D_GNU_SOURCE -O0 -g -pg -ggdb $(DEFINES)
endif

ifeq ($(mode),verbose)
  CFLAGS = -Wall -std=c99 -D_GNU_SOURCE -O0 -g $(DEFINES)
endif


CC=gcc
LDIR=./liblsys

RM   = rm

all:	lib
	$(CC) $(CFLAGS) $(OPTS) -L$(LDIR) $(LIBS) main.test.c -o main 

glfw:   lib
	$(CC) main.glfw.c -o main $(CFLAGS) $(OPTS) -L$(LDIR) $(LIBS) 

lib:
	$(MAKE) mode=$(mode) -C $(LDIR) 
clean:
	-$(RM) -f main main.test.o

