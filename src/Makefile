uname := $(shell sh -c 'uname -s 2>/dev/null')

ifeq ($(uname),Darwin)
  GL_DIR = "OpenGL"
endif

ifeq ($(uname),Linux)
  GL_DIR = "GL"
endif

DEFINES=-DGL_DIR=$(GL_DIR) 

CFLAGS = -Wall -std=c99 -D_GNU_SOURCE -O3 $(DEFINES)

ifeq ($(gles),true)
  DEFINES=-DGLES=1 $(DEFINES)
endif

ifeq ($(mode),prof)
  CFLAGS = -Wall -std=c99 -D_GNU_SOURCE -O0 -g -ggdb -pg $(DEFINES)
endif

ifeq ($(mode),debug)
  CFLAGS = -Wall -std=c99 -D_GNU_SOURCE -O0 -ggdb -pg -DDEBUG=1 $(DEFINES)
endif 

ifeq ($(mode),verbose)
  CFLAGS = -Wall -std=c99 -D_GNU_SOURCE -O0 -ggdb -pg -DDEBUG=1 -DVERBOSE=1 $(DEFINES)
endif

BISON_FILE_IN=parser.lsys.y
BISON_FILE_OUT_C=y.tab.c
BISON_FILE_OUT_H=y.tab.h

FLEX_FILE_IN=lexer.lsys.l
FLEX_FILE_OUT_C=lex.yy.c
FLEX_FILE_OUT_H=flex.liblsys.h

INTERMEDIATES := $(FLEX_FILE_OUT_C) $(BISON_FILE_OUT_C)
#C_FILES := $(filter-out $(INTERMEDIATES), $(wildcard *.c))
C_FILES := $(wildcard *.c)
OBJS := $(patsubst %.c,%.o, $(C_FILES))
     
CC=gcc
RM=rm

lib: $(OBJS) 
	rm -f liblsys.a && ar -r -s liblsys.a $(OBJS) y.tab.o lex.yy.o

$(OBJS): y.tab.o lex.yy.o

lex.yy.o: $(FLEX_FILE_OUT_C) $(BISON_FILE_OUT_C)
	$(CC) $(CFLAGS) -c -o $@ $<

y.tab.o: $(BISON_FILE_OUT_C)
	$(CC) $(CFLAGS) -c -o $@ $<

$(FLEX_FILE_OUT_C): $(BISON_FILE_OUT_C) $(FLEX_FILE_IN)
	flex $(FLEX_FILE_IN) 

$(BISON_FILE_OUT_C): $(BISON_FILE_IN) 
	bison $(BISON_FILE_IN) 

clean:
	-$(RM) -f $(OBJS) $(FLEX_FILE_OUT_C) $(FLEX_FILE_OUT_H) $(BISON_FILE_OUT_C) $(BISON_FILE_OUT_H)  
