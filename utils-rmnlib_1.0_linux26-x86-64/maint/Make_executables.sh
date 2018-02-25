#!/bin/bash
Where=$0
Where=$(readlink -e $Where)
Where=${Where%/bin/*}/Sources
echo Where=$Where
cd $Where
for i in * ; do ( cd $i ; make clean ; make LIBRMN=rmn_016.2.1 ; mv *${BASE_ARCH} ../../bin ; make clean ; ) ; done
cd ../bin
for i in *${BASE_ARCH} ; do ln -sf $i  $(echo $i | sed "s/_[^_]*${BASE_ARCH}$//" ) ; done
