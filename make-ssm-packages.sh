#!/bin/bash
Where=${0}
Where=${Where%/*}
cd ${Where} || exit 1
for Target in *_*_* ; do
  [[ ${Target} == *.ssm ]] && continue
  tar zcf ${Target}.ssm --exclude-tag=.ignore ${Target}
done
