
.SUFFIXES :

.SUFFIXES : .ftn .f .c .o

SHELL = /bin/sh

CPP = /lib/cpp

FFLAGS =

CFLAGS =

OPTIMIZ = -O 2

MASSVLIB = libmassvp4.a

.PRECIOUS: $(MASSVLIB)

genlib: 
#Creer ou mettre a jour la programmatheque 
	mkdir -p $(EC_ARCH)
	touch $(BASE_ARCH)/.ignore
	s.f90 -c -O 2 massvp4.F
	rm -f $(EC_ARCH)/$(MASSVLIB)
	ar rcv $(EC_ARCH)/$(MASSVLIB) massvp4.o
	rm massvp4.o

updlib:  genlib
#publier la programmatheque 
	mkdir -p ../lib/$(EC_ARCH)
	mv $(EC_ARCH)/$(MASSVLIB) ../lib/$(EC_ARCH)/$(MASSVLIB)
	cd ../lib/$(EC_ARCH)/ ; ln -sf $(MASSVLIB) libmassv_p4.a

clean:
	rm -f *.o $(EC_ARCH)/$(MASSVLIB)
	rm -rf $(EC_ARCH)
