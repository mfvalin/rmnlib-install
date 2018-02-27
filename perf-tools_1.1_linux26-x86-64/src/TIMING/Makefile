
LIBDIR = ../../lib

library:
	gcc -O2 -c -fopenmp hpcs_fort_hpm.c weak*.c rebind.c reporter.c history_trace.c dead_process_timer.c 
	ar rv libhpcsperf.a hpcs_fort_hpm.o weak*.o rebind.o reporter.o history_trace.o dead_process_timer.o 
	rm -f hpcs_fort_hpm.o rebind.o history_trace.o reporter.o weak*.o dead_process_timer.o

clean:
	rm -f *.o libhpcsperf.a

distclean: clean
	rm -f ${LIBDIR}/libhpcsperf.a

install: library
	mkdir -p ${LIBDIR}
	mv libhpcsperf.a ${LIBDIR}/libhpcsperf.a

