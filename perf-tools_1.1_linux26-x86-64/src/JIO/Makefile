
BINDIR = ../../bin

all:
	-gcc -m32 -fopenmp  -fpic -shared -o s.jio-prof-linux-32.so jio-uni-2.c -ldl -lm
	gcc       -fopenmp  -fpic -shared -o s.jio-prof-linux-64.so jio-uni-2.c -ldl -lm

clean:
	rm -f *.so

distclean: clean
	rm -f ${BINDIR}/*.so

${BINDIR}:
	mkdir -p ${BINDIR}

install: ${BINDIR} all
	-mv s.jio-prof-linux-32.so ${BINDIR}
	mv s.jio-prof-linux-64.so ${BINDIR}

