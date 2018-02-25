
SHELL = bash

.SUFFIXES:

.SUFFIXES : .c .ftn .f .o .cdk .ftn90 .f90

FFLAGS =

CFLAGS =

OPTIMIZ = -O 2

CPPFLAGS = 

VER = 16.0

PROGRAM = r.basename

PROGRAMS = r.basename r.ls r.isowner r.echo r.split kmwtopcl kmwtohp r.a2ps text2ps outine lpage nl2crlf traffic\
        arbre grepdep r.ucanrm r.microsleep process_watch host_ip_address\
        r.read_link rrbx2ppm r.resetenv

LIBRMN = rmn_015.2

#default: allbin
default: wlibrmn

.ftn90.o:
	s.compile $(OPTIMIZ) -opt "=$(FFLAGS)" -src $<

.ftn.o:
	s.compile $(OPTIMIZ) -opt "=$(FFLAGS)" -src $<

.f90.o:
	s.compile $(OPTIMIZ) -opt "=$(FFLAGS)" -src $<

.c.o:
	s.compile $(OPTIMIZ) -opt "=$(CFLAGS)" -src $<

.f.o:
	s.compile $(OPTIMIZ) -opt "=$(FFLAGS)" -src $<

Linux:  clean
	r.remotemake -as_armnlib Linux $(PROGRAM) RELS=$(RELS)
	r.distribute -as_armnlib -rels_$(RELS) Linux $(PROGRAM)

SGI:    clean
	r.remotemake -as_armnlib IRIX64 $(PROGRAM) RELS=$(RELS)
	r.distribute -as_armnlib -rels_$(RELS) IRIX64 $(PROGRAM)
#	r.remotemake -as_armnlib IRIX $(PROGRAM) RELS=$(RELS)
#	r.distribute -as_armnlib -rels_$(RELS) IRIX $(PROGRAM)

HP:     clean
	r.remotemake -as_armnlib HP-UX $(PROGRAM) RELS=$(RELS)
	r.distribute -as_armnlib -rels_$(RELS) HP-UX $(PROGRAM)

SX4:    clean
	r.remotemake -as_armnlib SX4 $(PROGRAM) RELS=$(RELS)
	r.distribute -as_armnlib -rels_$(RELS) -newbin SX4 $(PROGRAM)

SX5:    clean
	r.remotemake -as_armnlib SX5 $(PROGRAM) RELS=$(RELS)
	r.distribute -as_armnlib -rels_$(RELS) -newbin SX5 $(PROGRAM)

all:	Linux SGI HP SX4 SX5

r.basename: r.basename.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj r.basename.o -conly

r.ls: r.ls.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj r.ls.o -conly

r.isowner: r.isowner.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj r.isowner.o -conly

r.echo: r.echo.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj r.echo.o -conly

r.split: spliter.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj spliter.o -conly

kmwtopcl: kmwtopcl.o newdx.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj kmwtopcl.o newdx.o -conly

kmwtohp: kmwtohp.o newdx.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj kmwtohp.o newdx.o -conly

r.a2ps: a2ps.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj a2ps.o -conly

text2ps: text2ps.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj text2ps.o -conly

outine: outine.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj outine.o -conly

lpage: lpage.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj lpage.o -conly

nl2crlf: nl2crlf.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj nl2crlf.o -conly

traffic: traffic.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj traffic.o -conly

r.crackres: crackres.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj crackres.o -librmn $(LIBRMN)

arbre: arbre.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj arbre.o -conly

grepdep: grepdep.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj grepdep.o -conly

r.ucanrm: r.ucanrm.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj r.ucanrm.o -conly

wa_server: socket_wa_server.o
	s.compile -defines="-Dmain=cmain -DDEBUG=debug"  -src socket_wa_server.c -o $@_$(VER)-$(BASE_ARCH) -bidon c -main cmain -librmn $(LIBRMN)

r.read_link: r.read_link.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj r.read_link.o -conly

host_ip_address: host_ip_address.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj host_ip_address.o -conly

r.seq: r.seq.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj r.seq.o -conly

r.microsleep: r.microsleep.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj r.microsleep.o -conly

rrbx2ppm: rrbx2ppm.o
	s.compile -o $@_$(VER)-$(BASE_ARCH) -obj rrbx2ppm.o 

# replace EC_ARCH with BASE_ARCH for executable name
r.resetenv: r.resetenv.c
	r.compile -conly -librmn $(LIBRMN) -o $@_$(BASE_ARCH) -src r.resetenv.c

r.makedirs:
	gcc makedir.c -o $@_$(VER)-$(BASE_ARCH)	

allbin: r.basename r.ls r.isowner r.echo r.split kmwtopcl kmwtohp r.a2ps text2ps outine lpage nl2crlf traffic\
        arbre grepdep r.ucanrm r.read_link host_ip_address r.seq wa_server r.microsleep r.crackres rrbx2ppm r.resetenv r.makedirs

slibrmn: r.basename r.ls r.isowner r.echo r.split kmwtopcl kmwtohp r.a2ps text2ps outine lpage nl2crlf traffic\
        arbre grepdep r.ucanrm r.read_link host_ip_address r.seq r.microsleep r.makedirs

wlibrmn: r.crackres wa_server
	./Make.current_job_wall_clock_time $(LIBRMN)
# replace usage of r.basename with ${i%.ftn}
clean:
	-if [ "*.ftn" != "`echo *.ftn`" ] ; \
	then \
	for i in *.ftn ; \
	do \
	fn=$${i%.ftn} ; \
	rm -f $$fn.f; \
	done \
	fi
	-rm *_$(VER)-$(BASE_ARCH) *.o

