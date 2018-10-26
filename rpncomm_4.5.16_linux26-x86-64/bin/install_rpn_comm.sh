#!/bin/bash
export USE_RECURSIVE_SHORTCUTS=yes
Where=$(readlink -e ${0})
cd ${Where%/bin*}/src || exit 1
cd rpncomm || exit 1
. ./RPN_COMM_version.inc
export INSTALLED_VERSION=${RPN_COMM_version}
for Target in ${1:-/dev/gfortran} ; do
  echo "===== COMPILING and INSTALLING for architecture = $Target ====="
  ( \
    . r.load.dot ${Target} ; \
    [[ -n ${EC_ARCH} ]] || exit 1 ; \
    make clean ; \
    make itf ; \
    make all stublib ; \
    [[ -f ../lib/${EC_ARCH}/librpn_comm_${INSTALLED_VERSION}.a ]] ||  { echo "ERROR: failed to create librpn_comm" ; exit 1 ; } ; \
    [[ -f ../lib/${EC_ARCH}/librpn_commstubs_${INSTALLED_VERSION}.a ]] ||  { echo "ERROR: failed to create librpn_comm_stubs" ; exit 1 ; } ; \
    nobjects=$(ar tv ../lib/${EC_ARCH}/librpn_comm_${INSTALLED_VERSION}.a | wc -l) ; \
    make checkref ||  { echo "ERROR: missing/extra objects in librpn_comm_${INSTALLED_VERSION}.a" ; exit 1 ; } ; \
    mkdir -p ../../lib/${EC_ARCH} ; \
    mkdir -p ../../include/${EC_ARCH} ; \
    cp ../include/${EC_ARCH}/* ../../include/${EC_ARCH} ; \
    mv ../lib/${EC_ARCH}/librpn_comm_${INSTALLED_VERSION}.a ../../lib/${EC_ARCH} ; \
    mv ../lib/${EC_ARCH}/librpn_commstubs_${INSTALLED_VERSION}.a ../../lib/${EC_ARCH} ; \
    pushd ../../lib/${EC_ARCH} ; \
    echo "===== library contains ${nobjects} objects ====="
    for i in librpn_comm_4051609.a librpn_comm.a ; do ln -sf librpn_comm_${INSTALLED_VERSION}.a ${i} ; done ; \
    for i in librpn_commstubs_4051609.a librpn_commstubs.a ; do ln -sf librpn_commstubs_${INSTALLED_VERSION}.a ${i} ; done ; \
    popd ; \
    make clean ; \
  )
done

