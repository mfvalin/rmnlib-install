#!/bin/bash
Where=$(readlink -e ${0})
cd ${Where%/bin*}/source || exit 1
export USE_RECURSIVE_SHORTCUTS=yes
for Target in ${1:-/dev/gfortran} ; do
  echo "===== INSTALLING for architecture = $Target ====="
  ( \
    . r.load.dot ${Target} ; \
    [[ -n ${EC_ARCH} ]] || exit 1 ; \
    make  ;\
    )
done
