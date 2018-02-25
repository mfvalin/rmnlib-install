ifeq ($(EC_ARCH),$(BASE_ARCH))
 $(error EC_ARCH same as BASE_ARCH)
endif

#LIBRMN=rmn_016.3.1
LIBRMN=rmn

BINDIST = bindist

all: ${BINDIST} \
	${BINDIST}/bemol ${BINDIST}/editfst ${BINDIST}/pgsm ${BINDIST}/fstcomp \
	${BINDIST}/fstcompress ${BINDIST}/fststat ${BINDIST}/r.date \
	${BINDIST}/voir ${BINDIST}/r.ip1 ${BINDIST}/r.ip123 ${BINDIST}/editbrp \
	${BINDIST}/r.fstinfo ${BINDIST}/r.hy2pres ${BINDIST}/reflex \
	${BINDIST}/clonage  ${BINDIST}/dbzono \
	${BINDIST}/r.fstinfo ${BINDIST}/ip1newstyle ${BINDIST}/cmc_w_log

extra: ${BINDIST}/fst2xml ${BINDIST}/r.dict

${BINDIST}:
	mkdir -p ${BINDIST}

${BINDIST}/r.date: r.date
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make clean
	touch $@

${BINDIST}/r.hy2pres: r.hy2press
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make clean
	touch $@

${BINDIST}/r.ip123: r.ip123
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make clean
	touch $@

${BINDIST}/r.dict: r.dict
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make --ignore-errors clean
	touch $@

${BINDIST}/cmc_w_log: cmc_log
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%_*${BASE_ARCH}} ; \
	done && \
	make --ignore-errors clean
	touch $@

${BINDIST}/fst2xml: fstxml
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make --ignore-errors clean
	touch $@

${BINDIST}/r.fstinfo: fstinfo
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make --ignore-errors clean
	touch $@

${BINDIST}/ip1newstyle: ip1newstyle
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make --ignore-errors clean
	touch $@

${BINDIST}/clonage: clonage
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make --ignore-errors clean
	touch $@

${BINDIST}/dbzono: dbzono
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make --ignore-errors clean
	touch $@

${BINDIST}/reflex: reflex
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make --ignore-errors clean
	touch $@

${BINDIST}/voir: voir
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make --ignore-errors clean
	touch $@

${BINDIST}/r.ip1: r.ip1
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make --ignore-errors clean
	touch $@

${BINDIST}/bemol: bemol
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make clean
	touch $@

${BINDIST}/editbrp: editbrp
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make --ignore-errors clean
	touch $@

${BINDIST}/editfst: editfst
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make clean
	touch $@

${BINDIST}/pgsm: pgsm
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make clean
	touch $@

${BINDIST}/fstcomp: fstcomp
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make clean
	touch $@

${BINDIST}/fstcompress: fstcompress
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make clean
	touch $@

${BINDIST}/fststat: fststat
	make -C $< LIBRMN=$(LIBRMN)
	cd $< && \
  	for Target in *-${BASE_ARCH} ; do \
	  mv $${Target} ../${BINDIST}/$${Target%-${BASE_ARCH}} && \
	  ln -sf $${Target%-${BASE_ARCH}} ../${BINDIST}/$${Target%%_*} ; \
	done && \
	make clean
	touch $@

r.ip1_abs:
	make -C r.ip1 LIBRMN=$(LIBRMN)
	cd r.ip1 ; mv *Linux_x86-64 ../${BINDIST}/.

r.ip123_abs:
	make -C r.ip123 LIBRMN=$(LIBRMN)
	cd r.ip123 ; mv *Linux_x86-64 ../${BINDIST}/.

r.date_abs:
	make -C r.date LIBRMN=$(LIBRMN)

r.hy2press_abs:
	make -C r.hy2press LIBRMN=$(LIBRMN)
	cd r.hy2press ; mv *Linux_x86-64 ../${BINDIST}/.

r.voir_abs:
	make -C voir -f Makefile+ LIBRMN=$(LIBRMN)
	make -C voir voir+ -f Makefile+ LIBRMN=$(LIBRMN)
	cd voir ; mv *Linux_x86-64 ../${BINDIST}/.

clean:
	-make -C r.ip1 clean
	-make -C r.ip123 clean
	-make -C r.date clean
	-make -C r.hy2press clean
	-make -C voir -f Makefile+ clean
