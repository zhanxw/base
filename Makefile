CPPFLAGS=-g
GINCLUDE=/home/zhanxw/karmaColor/csg/libcsg/
GLIB=/home/zhanxw/karmaColor/csg/libcsg/libcsg.a
BASE= String File
BASEHDR = $(BASE:=.h)
BASESRC = $(BASE:=.cpp) main.cpp
BASEOBJ = $(BASESRC:.cpp=.o)

all:main read1
.cpp.o:
	g++ $(CPPFLAGS) -c $*.cpp

main:main.cpp $(BASEOBJ)
	g++ $(CPPFLAGS) -o main $(BASEOBJ) \
	-I./zlib -L./zlib -lz \
	-I./bzip2 -L./bzip2 -lbz2
read1: read1.o
	g++ $(CPPFLAGS) -D__ZLIB_AVAILABLE__ -o $@ $< $(GLIB) -lz
read1.o: read1.cpp
	g++ $(CPPFLAGS) -D__ZLIB_AVAILABLE__ -c $< -I$(GINCLUDE)
clean: 
	rm main

