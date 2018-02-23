
SHELL = bash

BH_PULL_SOURCE_GIT_BRANCH=6.1.1

LIBRMNPATH = $(shell T=$$(s.locate --lib rmnshared) ; printf "$${T%/*})"

ifeq ($(BASE_ARCH),$(EC_ARCH))
   $(info ERROR: please setup the compilation environment by typing the folowing:)
   $(info . setup.dot or . setup_gpsc.dot)
   $(error )
endif

default: libdescrip.a

updlib: shared
	mkdir -p ../lib/${EC_ARCH}
	mv *.a *.so ../lib/${EC_ARCH}
	mkdir -p ../include/${EC_ARCH}
	cp *.h *.hf vgrid_version.* *.mod ../include/${EC_ARCH}

dependencies.mk: *.c *.F90 *.h *.hc
	ls *.hc *.c *.F90 | ./s.dependencies.pl > dependencies.mk ;\

include dependencies.mk
 
%.o: %.F90
	s.f90 -fpic -c $(MY_FFLAGS) -lrmn $<

%.o: %.c
	s.cc -fpic -c $(MY_CFLAGS) $<

libdescrip.a: $(OBJECTS) vgrid_version dependencies.mk
	rm -f libdescrip.a && ar rcv libdescrip.a *.o 

shared: libdescrip.a
	s.f90 -shared -o libdescripshared.so *.o -lrmnshared$(SHARED_RNM_EXT)  -Wl,-rpath,${LIBRMNPATH}

vgrid_version:
	echo "character(len=128) :: vgrid_descriptors_version = \"VGRID $(BH_PULL_SOURCE_GIT_BRANCH) $(COMP_ARCH) $(ORDENV_PLAT) $(shell date)\"" > vgrid_version.hf 
	echo                "char vgrid_descriptors_version[] = \"VGRID $(BH_PULL_SOURCE_GIT_BRANCH) $(COMP_ARCH) $(ORDENV_PLAT) $(shell date)\";" > vgrid_version.hc 

clean:
	rm -f *.o *.mod

distclean: clean
	rm -rf libdescrip.a libdescripshared.so  ../lib/${EC_ARCH} ../include/${EC_ARCH}

