#!/bin/bash
Where=$0
Where=$(readlink -e $Where)
Where=${Where%/bin/*}/Repositories
echo Where=$Where
cd $Where
export USE_RECURSIVE_SHORTCUTS=yes
for Target in ${1:-/dev/gfortran} ; do
  echo "===== COMPILING and INSTALLING for architecture = $Target ====="
  ( \
    . r.load.dot ${Target} ; \
    [[ -n ${EC_ARCH} ]] || exit 1 ; \
    make BINDIST=../bin ; \
  )
done
