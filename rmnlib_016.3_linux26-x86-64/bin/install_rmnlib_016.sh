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
    [[ -f ${EC_ARCH}/librmn_016.3.1.a ]] && mv ${EC_ARCH}/librmn_016.3.1.a ../lib/${EC_ARCH} ; \
    mkdir -p ../include/${EC_ARCH} ; \
    cp PUBLIC_INCLUDES/* ../include/${EC_ARCH} ; \
    cp PUBLIC_INCLUDES/* ../include ; \
    pushd ../lib/${EC_ARCH} ; \
    echo "===== library contains $(ar tv librmn_016.3.1.a | wc -l) objects ====="
    for i in librmn_016.2.a librmn_016.a librmn.a librmnMP_016.2.a ; do ln -sf librmn_016.3.1.a ${i} ; done ; \
    mkdir temp ; \
    cd temp || exit 1 ;\
    ar x ../librmn_016.3.1.a ; rm whiteboard_omp.o *ccard*.o ; s.f90 -shared -o ../librmnshared_016.3.1.so *.o ; cd .. ; rm -rf temp ; \
    for i in librmn_016.so librmn.so ; do ln -sf librmnshared_016.3.1.so ${i} ; done ; \
    popd ; \
    make distclean ; \
  )
done
