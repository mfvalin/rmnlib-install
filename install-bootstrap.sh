#!/bin/bash
#set -x
export INSTALL_HOME=${INSTALL_HOME:-${HOME}/ssm-domains-base}
export GIT_HOME=${GIT_HOME:-https://github.com/mfvalin}
export AUTO_COMPILE=YES
#
# check for necessary software
#
Fault=""
for Mandatory in gazou /bin/ksh93 python make git gfortran perl ; do
  which ${Mandatory} || { echo "${Mandatory} not found, CANNOT INSTALL" ; Fault=1 ; }
done
[[ -n ${Fault} ]] && exit 1
#
export NO_RPN_COMM=""
which mpif90 || { echo "MPI development tools not available, MPI tools will not be installed" ; export NO_RPN_COMM=1 ; }
#
if [[ -z $NO_RPN_COMM ]] ; then    # only needed to generate rpn_comm
  echo "perl modules File::Spec::Functions File::Basename URI::file Cwd  needed for install"
  for i in  File::Spec::Functions File::Basename URI::file Cwd  ; do perl -e "use 5.008_008; use strict; use $i" ; done >Failed 2>&1
  [[ -s Failed ]] && sed 's/INC .*/INC/' <Failed && rm -f Failed && echo "Missing PERL module(s), CANNOT INSTALL" && exit 1
fi
#
# install ssm itself
#
if [[ ! -d ${INSTALL_HOME}/setup/ssm_10.151 ]] ; then
  mkdir ${HOME}/tmp
  git clone ${GIT_HOME}/ssm_fork.git tmp/ssm_10.151_all
  mkdir -p ${HOME}/ssm_depot
  (cd tmp ; tar zcf ${HOME}/ssm_depot/ssm_10.151_all.ssm --exclude=.git ssm_10.151_all ; )
  tmp/ssm_10.151_all/bin/ssm-installer_10.151_all.sh \
     --domainHome ${INSTALL_HOME}/setup/ssm_10.151 \
     --ssmRepositoryUrl ${HOME}/ssm_depot \
     --defaultRepositorySource ${HOME}/ssm_depot
fi
export SSM_SYSTEM_DOMAIN_HOME=${INSTALL_HOME}/setup/ssm_10.151
export SSM_DOMAIN_HOME=${INSTALL_HOME}/setup/ssm_10.151
. ${SSM_DOMAIN_HOME}/etc/ssm.d/profile
ssm listd -d ${SSM_DOMAIN_HOME}
if [[ ! -d ${SSM_DOMAIN_HOME}/ssmuse_1.4.1_all ]] ; then
  git clone${GIT_HOME}/ssmuse_fork.git tmp/ssmuse_1.4.1_all
  (cd tmp ; tar zcf ${HOME}/ssm_depot/ssmuse_1.4.1_all.ssm --exclude=.git ssmuse_1.4.1_all ; )
  ssm install -d ${SSM_DOMAIN_HOME} -f ${HOME}/ssm_depot/ssmuse_1.4.1_all.ssm
  ssm publish -d ${SSM_DOMAIN_HOME} -p ssmuse_1.4.1_all
fi
ssm listd -d ${SSM_DOMAIN_HOME}
if [[ ! -d ${SSM_DOMAIN_HOME}/ssm-wrappers_1.0.u_all ]] ; then
  git clone${GIT_HOME}/ssm-wrappers tmp/ssm-wrappers_1.0.u_all
  (cd tmp ; tar zcf ${HOME}/ssm_depot/ssm-wrappers_1.0.u_all.ssm --exclude=.git ssm-wrappers_1.0.u_all ; )
  ssm install -d ${SSM_DOMAIN_HOME} -f ${HOME}/ssm_depot/ssm-wrappers_1.0.u_all.ssm
  ssm publish -d ${SSM_DOMAIN_HOME} -p ssm-wrappers_1.0.u_all
fi
ssm listd -d ${SSM_DOMAIN_HOME}
#
mkdir -p ${INSTALL_HOME}/setup/ssm_10.151/ssm_10.151_all/etc/ssm.d/support/platforms
if [[ ! -d ${INSTALL_HOME}/base_003/dot-profile-setup_2.0_all ]] ; then
  git clone${GIT_HOME}/dot-profile-setup tmp/dot-profile-setup_2.0_all
  (cd tmp ; tar zcf ${HOME}/ssm_depot/dot-profile-setup_2.0_all.ssm --exclude=.git dot-profile-setup_2.0_all ; )
  ssm created -d ${INSTALL_HOME}/base_003 --sources ${HOME}/ssm_depot
  ssm install -d ${INSTALL_HOME}/base_003 -f ${HOME}/ssm_depot/dot-profile-setup_2.0_all.ssm
  ssm publish -d ${INSTALL_HOME}/base_003 -p dot-profile-setup_2.0_all
fi
ssm listd -d ${SSM_DOMAIN_HOME}
. ssmuse-sh -d ${SSM_DOMAIN_HOME}
env | grep SSM

