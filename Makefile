kros=i686-buildroot-linux-uclibc-gcc

all: czesc libwsp.so

czesc: program.c 
	${kros} program.c -o czesc -ldl
libwsp.so: libbibl.o
	${kros} -shared -fPIC -o libwsp.so libbibl.o
libbibl.o: libbibl.c
	${kros} -c -fPIC libbibl.c -o libbibl.o