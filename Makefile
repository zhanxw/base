CPPFLAGS=-g
GINCLUDE=/home/zhanxw/karmaColor/csg/libcsg/
GLIB=/home/zhanxw/karmaColor/csg/libcsg/libcsg.a
all:main read1

main:main.cpp String.h 
	g++ $(CPPFLAGS) -o main main.cpp \
	-I./zlib -L./zlib -lz \
	-I./bzip2 -L./bzip2 -lbz2
read1: read1.o
	g++ $(CPPFLAGS) -D__ZLIB_AVAILABLE__ -o $@ $< $(GLIB) -lz
read1.o: read1.cpp
	g++ $(CPPFLAGS) -D__ZLIB_AVAILABLE__ -c $< -I$(GINCLUDE)
clean: 
	rm main

