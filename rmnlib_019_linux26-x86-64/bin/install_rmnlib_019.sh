#!/bin/bash
export USE_RECURSIVE_SHORTCUTS=yes
Where=$(readlink -e ${0})
cd ${Where%/bin*}/src || exit 1
for Target in ${1:-/dev/gfortran} ; do
  echo "===== COMPILING and INSTALLING for architecture = $Target ====="
  ( \
    . r.load.dot ${Target} ; \
    [[ -n ${EC_ARCH} ]] || exit 1 ; \
    make distclean ; \
    make genlib ; \
    mkdir -p ../lib/${EC_ARCH} ; \
    [[ -f ${EC_ARCH}/librmn_019.2.a ]] || { echo "ERROR: failed to create librmn_019.2.a" ; exit 1 ; } ; \
    nobjects=$(ar tv ${EC_ARCH}/librmn_019.2.a | wc -l) ; \
    ((${nobjects}>=282)) ||  { echo "ERROR: missing objects (expected 282, found ${nobjects}) in librmn_019.2.a" ; exit 1 ; } ; \
    mv ${EC_ARCH}/librmn_019.2.a ../lib/${EC_ARCH} ; \
    mkdir -p ../include/${EC_ARCH} ; \
    cp PUBLIC_INCLUDES/* ../include/${EC_ARCH} ; \
    cp PUBLIC_INCLUDES/* ../include ; \
    pushd ../lib/${EC_ARCH} ; \
    echo "===== library contains ${nobjects} objects ====="
    for i in librmn_019.2.a librmn_019.a librmn.a librmnMP_019.2.a ; do ln -sf librmn_019.2.a ${i} ; done ; \
    mkdir temp ; \
    cd temp || exit 1 ;\
    ar x ../librmn_019.2.a ; rm whiteboard_omp.o *ccard*.o ; s.f90 -shared -o ../librmnshared_019.2.so *.o ; cd .. ; rm -rf temp ; \
    for i in librmnshared_019.so librmnshared.so ; do ln -sf librmnshared_019.2.so ${i} ; done ; \
    popd ; \
    make distclean ; \
  )
done
