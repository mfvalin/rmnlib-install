
include rmnlib-install.cfg

SHELL = bash

SSM_VERBOSE = 

WITH_SSM_BASE    = . ${SSM_DOMAIN_HOME}/etc/ssm.d/profile

WITH_SSM_SETUP   = ${WITH_SSM_BASE} && . env-setup.dot && export GIT_CACHE=${GIT_CACHE}

WITH_LIB_SETUP   = ${WITH_SSM_SETUP} && . r.load.dot ${SSM_LIB_DOMAIN}

default: update

update:
	rm -f phase*
	make phase5

install:
	make phase5

GIT_ARMNLIB = https://github.com/armnlib

GIT_PACKAGES = \
	${GIT_CACHE}/ssm_fork.git \
	${GIT_CACHE}/ssmuse_fork.git \
	${GIT_CACHE}/ssm-wrappers.git \
	${GIT_CACHE}/dot-profile-setup \
	${GIT_CACHE}/shortcut-tools \
	${GIT_CACHE}/cmcarc_fork \
	${GIT_CACHE}/env-utils \
	${GIT_CACHE}/code-tools \
	${GIT_CACHE}/perl_needed \
	${GIT_CACHE}/librmn \
	${GIT_CACHE}/rpncomm \
	${GIT_CACHE}/bemol \
	${GIT_CACHE}/clonage \
	${GIT_CACHE}/cmc_log \
	${GIT_CACHE}/dbzono \
	${GIT_CACHE}/editbrp \
	${GIT_CACHE}/editfst \
	${GIT_CACHE}/fstcomp \
	${GIT_CACHE}/fstcompress \
	${GIT_CACHE}/fstinfo \
	${GIT_CACHE}/fststat \
	${GIT_CACHE}/fstxml \
	${GIT_CACHE}/ip1newstyle \
	${GIT_CACHE}/misc \
	${GIT_CACHE}/pgsm \
	${GIT_CACHE}/r.crc \
	${GIT_CACHE}/r.date \
	${GIT_CACHE}/r.dict \
	${GIT_CACHE}/r.hy2press \
	${GIT_CACHE}/r.ip1 \
	${GIT_CACHE}/r.ip123 \
	${GIT_CACHE}/reflex \
	${GIT_CACHE}/voir

gitcache: ${GIT_PACKAGES}

SSM_PACKAGES = \
	${SSM_REPOSITORY}/afsisio_1.0u_all.ssm \
	${SSM_REPOSITORY}/armnlib_2.0u_all.ssm \
	${SSM_REPOSITORY}/ssm_10.151_all.ssm \
	${SSM_REPOSITORY}/perl-needed_0.0_linux26-x86-64.ssm \
	${SSM_REPOSITORY}/ssmuse_1.4.1_all.ssm \
	${SSM_REPOSITORY}/ssm-wrappers_1.0.u_all.ssm \
	${SSM_REPOSITORY}/env-setup_003_all.ssm \
	${SSM_REPOSITORY}/dot-profile-setup_2.0_all.ssm \
	${SSM_REPOSITORY}/shortcut-tools_1.0_all.ssm \
	${SSM_REPOSITORY}/${SSM_SHORTCUTS}.ssm \
	${SSM_REPOSITORY}/cmcarc_4.3.1u_linux26-x86-64.ssm \
	${SSM_REPOSITORY}/env-utils_1.0u_all.ssm \
	${SSM_REPOSITORY}/code-tools_1.0_all.ssm \
	${SSM_REPOSITORY}/makebidon_1.1_linux26-x86-64.ssm \
	${SSM_REPOSITORY}/r.gppf_1.0.1_linux26-x86-64.ssm \
	${SSM_REPOSITORY}/massvp4_1.0_linux26-x86-64.ssm \
	${SSM_REPOSITORY}/rmnlib_016.3_linux26-x86-64.ssm \
	${SSM_REPOSITORY}/utils-rmnlib_1.0_linux26-x86-64.ssm \
	${SSM_REPOSITORY}/rpncomm_4.5.16_linux26-x86-64.ssm \
	${SSM_REPOSITORY}/vgrid_6.1.gnu_linux26-x86-64.ssm

ENV_PACKAGES = \
	${SSM_ENV_DOMAIN}/perl-needed_0.0_linux26-x86-64 \
	${SSM_DOMAIN_HOME}/ssmuse_1.4.1_all \
	${SSM_DOMAIN_HOME}/ssm-wrappers_1.0.u_all \
	${SSM_DOMAIN_HOME}/env-setup_003_all \
	${SSM_DOMAIN_HOME}/dot-profile-setup_2.0_all \
	${SSM_ENV_DOMAIN}/cmcarc_4.3.1u_linux26-x86-64 \
	${SSM_ENV_DOMAIN}/shortcut-tools_1.0_all \
	${SSM_ENV_DOMAIN}/env-utils_1.0u_all \
	${SSM_ENV_DOMAIN}/code-tools_1.0_all \
	${SSM_ENV_DOMAIN}/r.gppf_1.0.1_linux26-x86-64 \
	${SSM_ENV_DOMAIN}/afsisio_1.0u_all \
	${SSM_ENV_DOMAIN}/armnlib_2.0u_all \
	${SSM_ENV_DOMAIN}/${SSM_SHORTCUTS}

LIB_PACKAGES = \
	${SSM_LIB_DOMAIN}/massvp4_1.0_linux26-x86-64     \
	${SSM_LIB_DOMAIN}/rmnlib_016.3_linux26-x86-64    \
	${SSM_LIB_DOMAIN}/makebidon_1.1_linux26-x86-64   \
	${SSM_LIB_DOMAIN}/rpncomm_4.5.16_linux26-x86-64  \
	${SSM_LIB_DOMAIN}/vgrid_6.1.gnu_linux26-x86-64

##############################################################################################################
# phase 0 : populate the git cache and the ssm cache,  create package repository
##############################################################################################################
phase0: ${GIT_CACHE} dependencies.done ${SSM_CACHE} ${INSTALL_HOME} ${SSM_REPOSITORY}
	touch $@

##############################################################################################################
# phase 1 : create installation master directory, install ssm
##############################################################################################################
phase1: | phase0
	make phase1.done
	touch $@

phase1.done: rmnlib-install.dot ${INSTALL_HOME} ${SSM_DOMAIN_HOME}
	@printf '====================== phase 1 done ======================\n\n'
	touch $@

##############################################################################################################
# phase 2 : needs ssm
#           acquire ssm, create environment domain
#           install and publish ssmuse, ssm wrappers, user profile setup, setup utilities
#                               cmcarc, shortcuts, environment utilities, compiling tools
##############################################################################################################
phase2: | phase1 phase0
	${WITH_SSM_BASE} && make phase2.done
	touch $@

# packages armnlib(data+include) and afsisio to be added here
phase2.done:  ${SSM_ENV_DOMAIN} ${ENV_PACKAGES} \
	listd
	@printf '====================== phase 2 done ======================\n\n'
	touch $@

##############################################################################################################
# phase 3 : needs ssm, tools installed in phase 2, and a user setup (compilers and tools)
#           create domain for libraries, install and optionally compile libraries
##############################################################################################################
phase3: | phase2 phase1 phase0
	${WITH_SSM_SETUP} && make phase3.done
	touch $@

phase3.done: ${SSM_LIB_DOMAIN} ${LIB_PACKAGES} \
        listd liste
	@printf '====================== phase 3 done ======================\n\n'
	touch $@

##############################################################################################################
# phase 4 : needs ssm, tools and library sources installed in phases 2 and 3, 
#           and a user setup (compilers and tools)
#           compile and publish libraries
##############################################################################################################
phase4: | phase3 phase2 phase1 phase0 mpidependencies.done ${ENV_PACKAGES}
	${WITH_LIB_SETUP} && make phase4.done
	touch $@

phase4.done: massvp4.done rmnlib.done makebidon.done rpncomm.done vgrid.done utilities.done
	@printf '====================== phase 4 done ======================\n\n'
	touch $@

##############################################################################################################
# phase 5 : needs ssm, tools and libraries installed in phases 2, 3 and 4
#           and a full user setup (compilers, tools and libraries)
#           compile and publish tools
##############################################################################################################
phase5: | phase4 phase3 phase2 phase1 phase0 mpidependencies.done ${ENV_PACKAGES}
	${WITH_LIB_SETUP} && make phase5.done
	touch $@

phase5.done:
	@printf '====================== phase 5 done ======================\n\n'
	touch $@

##############################################################################################################

wipe_install:
	rm -rf ${INSTALL_HOME}/*
	rm -rf ${SSM_REPOSITORY}/*
	rm -f *.done phase?
listd:
	@ssm listd -d ${SSM_DOMAIN_HOME}

liste:
	ssm listd -d ${SSM_ENV_DOMAIN}

${GIT_CACHE}:
	@echo "PLS create directory $@ (need ~50MBytes)" ; false

${GIT_CACHE}/bemol:
	git clone ${GIT_ARMNLIB}/bemol ${GIT_CACHE}/bemol

${GIT_CACHE}/clonage:
	git clone ${GIT_ARMNLIB}/clonage ${GIT_CACHE}/clonage

${GIT_CACHE}/cmc_log:
	git clone ${GIT_ARMNLIB}/cmc_log ${GIT_CACHE}/cmc_log

${GIT_CACHE}/dbzono:
	git clone ${GIT_ARMNLIB}/dbzono ${GIT_CACHE}/dbzono

${GIT_CACHE}/editbrp:
	git clone ${GIT_ARMNLIB}/editbrp ${GIT_CACHE}/editbrp

${GIT_CACHE}/editfst:
	git clone ${GIT_ARMNLIB}/editfst ${GIT_CACHE}/editfst

${GIT_CACHE}/fstcomp:
	git clone ${GIT_ARMNLIB}/fstcomp ${GIT_CACHE}/fstcomp

${GIT_CACHE}/fstcompress:
	git clone ${GIT_ARMNLIB}/fstcompress ${GIT_CACHE}/fstcompress

${GIT_CACHE}/fstinfo:
	git clone ${GIT_ARMNLIB}/fstinfo ${GIT_CACHE}/fstinfo

${GIT_CACHE}/fststat:
	git clone ${GIT_ARMNLIB}/fststat ${GIT_CACHE}/fststat

${GIT_CACHE}/fstxml:
	git clone ${GIT_ARMNLIB}/fstxml ${GIT_CACHE}/fstxml

${GIT_CACHE}/ip1newstyle:
	git clone ${GIT_ARMNLIB}/ip1newstyle ${GIT_CACHE}/ip1newstyle

${GIT_CACHE}/misc:
	git clone ${GIT_ARMNLIB}/misc ${GIT_CACHE}/misc

${GIT_CACHE}/pgsm:
	git clone ${GIT_ARMNLIB}/pgsm ${GIT_CACHE}/pgsm

${GIT_CACHE}/r.crc:
	git clone ${GIT_ARMNLIB}/r.crc ${GIT_CACHE}/r.crc

${GIT_CACHE}/r.date:
	git clone ${GIT_ARMNLIB}/r.date ${GIT_CACHE}/r.date

${GIT_CACHE}/r.dict:
	git clone ${GIT_ARMNLIB}/r.dict ${GIT_CACHE}/r.dict

${GIT_CACHE}/r.hy2press:
	git clone ${GIT_ARMNLIB}/r.hy2press ${GIT_CACHE}/r.hy2press

${GIT_CACHE}/r.ip1:
	git clone ${GIT_ARMNLIB}/r.ip1 ${GIT_CACHE}/r.ip1

${GIT_CACHE}/r.ip123:
	git clone ${GIT_ARMNLIB}/r.ip123 ${GIT_CACHE}/r.ip123

${GIT_CACHE}/reflex:
	git clone ${GIT_ARMNLIB}/reflex ${GIT_CACHE}/reflex

${GIT_CACHE}/voir:
	git clone ${GIT_ARMNLIB}/voir ${GIT_CACHE}/voir

${GIT_CACHE}/perl_needed:
	git clone ${GIT_HOME}/perl_needed ${GIT_CACHE}/perl_needed

${GIT_CACHE}/ssm_fork.git:
	git clone ${GIT_HOME}/ssm_fork ${GIT_CACHE}/ssm_fork.git

${GIT_CACHE}/ssmuse_fork.git:
	git clone ${GIT_HOME}/ssmuse_fork ${GIT_CACHE}/ssmuse_fork.git

${GIT_CACHE}/ssm-wrappers.git:
	git clone ${GIT_HOME}/ssm-wrappers ${GIT_CACHE}/ssm-wrappers.git

${GIT_CACHE}/dot-profile-setup:
	git clone ${GIT_HOME}/dot-profile-setup ${GIT_CACHE}/dot-profile-setup

${GIT_CACHE}/shortcut-tools:
	git clone ${GIT_HOME}/shortcut-tools ${GIT_CACHE}/shortcut-tools

${GIT_CACHE}/cmcarc_fork:
	git clone ${GIT_HOME}/cmcarc_fork ${GIT_CACHE}/cmcarc_fork

${GIT_CACHE}/env-utils:
	git clone ${GIT_HOME}/env-utils ${GIT_CACHE}/env-utils

${GIT_CACHE}/code-tools:
	git clone ${GIT_HOME}/code-tools ${GIT_CACHE}/code-tools

${GIT_CACHE}/librmn:
	git clone ${GIT_HOME}/librmn ${GIT_CACHE}/librmn
	cd ${GIT_CACHE}/librmn ; \
	for branch in `git branch -a | grep remotes | grep -v HEAD | grep -v master `; do    \
	  git branch --track $${branch#remotes/origin/} $$branch; \
	done

${GIT_CACHE}/rpncomm:
	git clone ${GIT_HOME}/rpncomm ${GIT_CACHE}/rpncomm
	cd ${GIT_CACHE}/rpncomm ; \
	for branch in `git branch -a | grep remotes | grep -v HEAD | grep -v master `; do    \
	  git branch --track $${branch#remotes/origin/} $$branch; \
	done

${SSM_CACHE}:
	@echo "PLS create directory $@ (need ~100MByte)" ; false

${SSM_CACHE}/afsisio_1.0u_all.ssm:
	cd ${SSM_CACHE} && wget ${WEB_HOME}/afsisio_1.0u_all.ssm

${SSM_CACHE}/armnlib_2.0u_all.ssm:
	cd ${SSM_CACHE} && wget ${WEB_HOME}/armnlib_2.0u_all.ssm


${SSM_REPOSITORY}/afsisio_1.0u_all.ssm: ${SSM_CACHE}/afsisio_1.0u_all.ssm
	ln -sf ${SSM_CACHE}/afsisio_1.0u_all.ssm ${SSM_REPOSITORY}/afsisio_1.0u_all.ssm

${SSM_REPOSITORY}/armnlib_2.0u_all.ssm: ${SSM_CACHE}/armnlib_2.0u_all.ssm
	ln -sf ${SSM_CACHE}/armnlib_2.0u_all.ssm ${SSM_REPOSITORY}/armnlib_2.0u_all.ssm

${INSTALL_HOME}:
	@echo "PLS create directory $@ (need ~500MByte)" ; false

dependencies.done: 
	@ls /usr/include/openssl/md5.h || { echo "ERROR: openssl/md5.h not found" ; exit 1 ; }
	@which git            || { echo "ERROR: git not found" ; exit 1 ; }
	@which /bin/ksh       || { echo "ERROR: /bin/ksh not found" ; exit 1 ; }
	@which /bin/ksh93     || { echo "ERROR: /bin/ksh93 not found" ; exit 1 ; }
	@echo "typeset -A aa; aa['tagada']='shimboum' ; typeset -Z4 n" | /bin/ksh93  || { echo "ERROR: not a bona fide ksh93" ; exit 1 ; }
	@gfortran --version >gfortran.version      || { echo "ERROR: gfortran not found" ; exit 1 ; }
	@mpif90 --version   >mpif90.version        || { echo "ERROR: mpif90 not found" ; exit 1 ; }
	@diff -q gfortran.version mpif90.version   || { echo "ERROR: mpif90/gfortran version not the same" ; exit 1 ; }
	@which python         || { echo "ERROR: python not found" ; exit 1 ; }
	@which perl           || { echo "ERROR: perl not found" ; exit 1 ; }
	@for i in  File::Spec::Functions File::Basename URI::file Cwd  ; do \
	    perl -e "use 5.008_008; use strict; use $$i" 2>/dev/null ||  \
	    { printf "ERROR: missing needed perl module $$i, try\n . ./get_perl_needed.dot ${GIT_CACHE}\n" ; exit 1 ; } \
	    done 
	touch $@

mpidependencies.done:
	@printf "program mpi\ncall mpi_init(ierr)\nprint *,'Hello World'\ncall mpi_finalize(ierr)\nstop\nend\n" >mpitest.F90
	@mpif90 -o mpitest.Abs mpitest.F90
	@mpirun -n 2 ./mpitest.Abs
	@rm -f mpitest.F90 mpitest.Abs
	touch $@

${SSM_REPOSITORY}:
	mkdir -p $@
	echo "created directory $@ ((need ~100MByte)"

${SSM_BASE_DOMAIN}:
	ssm created ${SSM_VERBOSE} -d $@ --sources ${SSM_REPOSITORY}

${SSM_ENV_DOMAIN}:
	ssm created ${SSM_VERBOSE} -d $@ --sources ${SSM_REPOSITORY}

${SSM_LIB_DOMAIN}:
	ssm created ${SSM_VERBOSE} -d $@ --sources ${SSM_REPOSITORY}

rmnlib-install.dot: rmnlib-install.cfg
	cat rmnlib-install.cfg | sed -e 's/^/export /'  -e 's/[ ]*=[ ]*/=/' >rmnlib-install.dot

${SSM_REPOSITORY}/ssm_10.151_all.ssm: ${GIT_CACHE}/ssm_fork.git
	cd ${SSM_REPOSITORY} && rm -rf ssm_10.151_all && \
	    git clone ${GIT_CACHE}/ssm_fork.git ssm_10.151_all && \
	    tar zcf ssm_10.151_all.ssm --exclude=.git ssm_10.151_all

${SSM_DOMAIN_HOME}: ${SSM_REPOSITORY}/ssm_10.151_all.ssm
	${SSM_REPOSITORY}/ssm_10.151_all/bin/ssm-installer_10.151_all.sh \
	    --label 'ssm package' --force \
	    --domainHome ${SSM_DOMAIN_HOME} \
	    --ssmRepositoryUrl ${SSM_REPOSITORY} \
	    --defaultRepositorySource ${SSM_REPOSITORY}

# perl-needed
${SSM_ENV_DOMAIN}/perl-needed_0.0_linux26-x86-64: ${SSM_REPOSITORY}/perl-needed_0.0_linux26-x86-64.ssm
	ssm install --clobber -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/perl-needed_0.0_linux26-x86-64.ssm
	( cd ${SSM_ENV_DOMAIN}/perl-needed_0.0_linux26-x86-64/src && make ; )
	ssm publish -d ${SSM_ENV_DOMAIN} -p perl-needed_0.0_linux26-x86-64 --force

${SSM_REPOSITORY}/perl-needed_0.0_linux26-x86-64.ssm: ${GIT_CACHE}/perl_needed
	cd ${SSM_REPOSITORY} && rm -rf perl-needed_0.0_linux26-x86-64 && \
	  git clone ${GIT_CACHE}/perl_needed perl-needed_0.0_linux26-x86-64 && \
	  tar zcf perl-needed_0.0_linux26-x86-64.ssm  --exclude=.git perl-needed_0.0_linux26-x86-64

# ssmuse_1.4.1_all
${SSM_DOMAIN_HOME}/ssmuse_1.4.1_all: ${SSM_REPOSITORY}/ssmuse_1.4.1_all.ssm
	ssm install --clobber -d ${SSM_DOMAIN_HOME} -f ${SSM_REPOSITORY}/ssmuse_1.4.1_all.ssm
	ssm publish -d ${SSM_DOMAIN_HOME} -p ssmuse_1.4.1_all --force

${SSM_REPOSITORY}/ssmuse_1.4.1_all.ssm: ${GIT_CACHE}/ssmuse_fork.git
	cd ${SSM_REPOSITORY} && rm -rf ssmuse_1.4.1_all && \
	    git clone ${GIT_CACHE}/ssmuse_fork.git ssmuse_1.4.1_all && \
	    tar zcf ssmuse_1.4.1_all.ssm --exclude=.git ssmuse_1.4.1_all

# ssm-wrappers_1.0.u_all
${SSM_DOMAIN_HOME}/ssm-wrappers_1.0.u_all: ${SSM_REPOSITORY}/ssm-wrappers_1.0.u_all.ssm
	ssm install --clobber -d ${SSM_DOMAIN_HOME} -f ${SSM_REPOSITORY}/ssm-wrappers_1.0.u_all.ssm
	ssm publish -d ${SSM_DOMAIN_HOME} -p ssm-wrappers_1.0.u_all --force

${SSM_REPOSITORY}/ssm-wrappers_1.0.u_all.ssm: ${GIT_CACHE}/ssm-wrappers.git
	cd ${SSM_REPOSITORY} && rm -rf ssm-wrappers_1.0.u_all && \
	    git clone ${GIT_CACHE}/ssm-wrappers.git ssm-wrappers_1.0.u_all && \
	    tar zcf ssm-wrappers_1.0.u_all.ssm --exclude=.git ssm-wrappers_1.0.u_all

#env-setup_003_all
${SSM_DOMAIN_HOME}/env-setup_003_all: ${SSM_REPOSITORY}/env-setup_003_all.ssm
	ssm install --clobber -d ${SSM_DOMAIN_HOME} -f ${SSM_REPOSITORY}/env-setup_003_all.ssm
	ssm publish -d ${SSM_DOMAIN_HOME} -p env-setup_003_all --force

${SSM_REPOSITORY}/env-setup_003_all.ssm: env-setup_003_all
	tar zcf ${SSM_REPOSITORY}/env-setup_003_all.ssm env-setup_003_all

# dot-profile-setup_2.0_all
${SSM_DOMAIN_HOME}/dot-profile-setup_2.0_all: ${SSM_REPOSITORY}/dot-profile-setup_2.0_all.ssm
	ssm install --clobber -d ${SSM_DOMAIN_HOME} -f ${SSM_REPOSITORY}/dot-profile-setup_2.0_all.ssm
	ssm publish -d ${SSM_DOMAIN_HOME} -p dot-profile-setup_2.0_all --force

${SSM_REPOSITORY}/dot-profile-setup_2.0_all.ssm: ${GIT_CACHE}/dot-profile-setup
	cd ${SSM_REPOSITORY} && rm -rf dot-profile-setup_2.0_all && \
	    git clone ${GIT_CACHE}/dot-profile-setup dot-profile-setup_2.0_all && \
	    tar zcf dot-profile-setup_2.0_all.ssm --exclude=.git dot-profile-setup_2.0_all

#shortcut-tools_1.0_all
${SSM_ENV_DOMAIN}/shortcut-tools_1.0_all: ${SSM_REPOSITORY}/shortcut-tools_1.0_all.ssm
	ssm install --clobber -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/shortcut-tools_1.0_all.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p shortcut-tools_1.0_all --force

${SSM_REPOSITORY}/shortcut-tools_1.0_all.ssm: ${GIT_CACHE}/shortcut-tools
	cd ${SSM_REPOSITORY} && rm -rf shortcut-tools_1.0_all && \
	    git clone ${GIT_CACHE}/shortcut-tools shortcut-tools_1.0_all && \
	    tar zcf shortcut-tools_1.0_all.ssm --exclude=.git shortcut-tools_1.0_all

# shortcuts (generic or site specific)
${SSM_ENV_DOMAIN}/${SSM_SHORTCUTS}: ${SSM_REPOSITORY}/${SSM_SHORTCUTS}.ssm
	ssm install --clobber -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/${SSM_SHORTCUTS}.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p ${SSM_SHORTCUTS} --force

${SSM_REPOSITORY}/${SSM_SHORTCUTS}.ssm: ${SSM_SHORTCUTS}
	tar zcf ${SSM_REPOSITORY}/${SSM_SHORTCUTS}.ssm ${SSM_SHORTCUTS}

#cmcarc_4.3.1u_linux26-x86-64
${SSM_ENV_DOMAIN}/cmcarc_4.3.1u_linux26-x86-64: ${SSM_REPOSITORY}/cmcarc_4.3.1u_linux26-x86-64.ssm
	ssm install --clobber -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/cmcarc_4.3.1u_linux26-x86-64.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p cmcarc_4.3.1u_linux26-x86-64 --force

${SSM_REPOSITORY}/cmcarc_4.3.1u_linux26-x86-64.ssm: ${GIT_CACHE}/cmcarc_fork
	cd ${SSM_REPOSITORY} && rm -rf cmcarc_4.3.1u_linux26-x86-64 && \
	    git clone ${GIT_CACHE}/cmcarc_fork cmcarc_4.3.1u_linux26-x86-64 && \
	    tar zcf cmcarc_4.3.1u_linux26-x86-64.ssm --exclude=.git cmcarc_4.3.1u_linux26-x86-64

#env-utils_1.0u_all
${SSM_ENV_DOMAIN}/env-utils_1.0u_all: ${SSM_REPOSITORY}/env-utils_1.0u_all.ssm
	ssm install --clobber -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/env-utils_1.0u_all.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p env-utils_1.0u_all --force

${SSM_REPOSITORY}/env-utils_1.0u_all.ssm: ${GIT_CACHE}/env-utils
	cd ${SSM_REPOSITORY} && rm -rf env-utils_1.0u_all && \
	    git clone ${GIT_CACHE}/env-utils env-utils_1.0u_all && \
	    tar zcf env-utils_1.0u_all.ssm --exclude=.git env-utils_1.0u_all

#code-tools_1.0_all
${SSM_ENV_DOMAIN}/code-tools_1.0_all: ${SSM_REPOSITORY}/code-tools_1.0_all.ssm
	ssm install --clobber -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/code-tools_1.0_all.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p code-tools_1.0_all --force

${SSM_REPOSITORY}/code-tools_1.0_all.ssm: ${GIT_CACHE}/code-tools
	cd ${SSM_REPOSITORY} && rm -rf code-tools_1.0_all && \
	    git clone ${GIT_CACHE}/code-tools code-tools_1.0_all && \
	    tar zcf code-tools_1.0_all.ssm --exclude=.git code-tools_1.0_all

# r.gppf_1.0.1_linux26-x86-64
${SSM_ENV_DOMAIN}/r.gppf_1.0.1_linux26-x86-64: ${SSM_REPOSITORY}/r.gppf_1.0.1_linux26-x86-64.ssm
	ssm install --clobber -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/r.gppf_1.0.1_linux26-x86-64.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p r.gppf_1.0.1_linux26-x86-64 --force

${SSM_REPOSITORY}/r.gppf_1.0.1_linux26-x86-64.ssm: r.gppf_1.0.1_linux26-x86-64
	tar zcf ${SSM_REPOSITORY}/r.gppf_1.0.1_linux26-x86-64.ssm --exclude=.git r.gppf_1.0.1_linux26-x86-64

# massvp4
massvp4.done: ${SSM_LIB_DOMAIN}/massvp4_1.0_linux26-x86-64
	install_massvp4.sh ${DEFAULT_INSTALL_ARCH}
	ssm publish -d ${SSM_LIB_DOMAIN} -p massvp4_1.0_linux26-x86-64 --force
	touch $@

${SSM_LIB_DOMAIN}/massvp4_1.0_linux26-x86-64: ${SSM_REPOSITORY}/massvp4_1.0_linux26-x86-64.ssm
	ssm install --clobber -d ${SSM_LIB_DOMAIN} -f ${SSM_REPOSITORY}/massvp4_1.0_linux26-x86-64.ssm
	ssm publish -d ${SSM_LIB_DOMAIN} -p massvp4_1.0_linux26-x86-64 --force
	touch $@

${SSM_REPOSITORY}/massvp4_1.0_linux26-x86-64.ssm: massvp4_1.0_linux26-x86-64
	tar zcf ${SSM_REPOSITORY}/massvp4_1.0_linux26-x86-64.ssm --exclude=.git massvp4_1.0_linux26-x86-64
	rm -f massvp4.done

# rmnlib
rmnlib.done: ${SSM_LIB_DOMAIN}/rmnlib_016.3_linux26-x86-64
	install_rmnlib_016.sh ${DEFAULT_INSTALL_ARCH}
	ssm publish -d ${SSM_LIB_DOMAIN} -p rmnlib_016.3_linux26-x86-64 --force
	touch $@

${SSM_LIB_DOMAIN}/rmnlib_016.3_linux26-x86-64: ${SSM_REPOSITORY}/rmnlib_016.3_linux26-x86-64.ssm
	ssm install --clobber -d ${SSM_LIB_DOMAIN} -f ${SSM_REPOSITORY}/rmnlib_016.3_linux26-x86-64.ssm
	ssm publish -d ${SSM_LIB_DOMAIN} -p rmnlib_016.3_linux26-x86-64 --force
	touch $@

${SSM_REPOSITORY}/rmnlib_016.3_linux26-x86-64.ssm: rmnlib_016.3_linux26-x86-64 ${GIT_CACHE}/librmn
	tar zcf ${SSM_REPOSITORY}/rmnlib_016.3_linux26-x86-64.ssm --exclude=.git rmnlib_016.3_linux26-x86-64
	rm -f rmnlib.done

# rmnlib associated utilities
utilities.done: ${SSM_LIB_DOMAIN}/utils-rmnlib_1.0_linux26-x86-64
	install_rmnlib_utils.sh ${DEFAULT_INSTALL_ARCH}
	ssm publish -d ${SSM_LIB_DOMAIN} -p utils-rmnlib_1.0_linux26-x86-64 --force
	touch $@

${SSM_LIB_DOMAIN}/utils-rmnlib_1.0_linux26-x86-64: ${SSM_REPOSITORY}/utils-rmnlib_1.0_linux26-x86-64.ssm
	ssm install --clobber -d ${SSM_LIB_DOMAIN} -f ${SSM_REPOSITORY}/utils-rmnlib_1.0_linux26-x86-64.ssm
	ssm publish -d ${SSM_LIB_DOMAIN} -p utils-rmnlib_1.0_linux26-x86-64 --force
	touch $@

${SSM_REPOSITORY}/utils-rmnlib_1.0_linux26-x86-64.ssm: utils-rmnlib_1.0_linux26-x86-64 ${GIT_CACHE}/librmn
	tar zcf ${SSM_REPOSITORY}/utils-rmnlib_1.0_linux26-x86-64.ssm --exclude=.git utils-rmnlib_1.0_linux26-x86-64
	rm -f utilities.done

#makebidon
makebidon.done: ${SSM_LIB_DOMAIN}/makebidon_1.1_linux26-x86-64
	install_makebidon.sh ${DEFAULT_INSTALL_ARCH}
	ssm publish -d ${SSM_LIB_DOMAIN} -p makebidon_1.1_linux26-x86-64 --force
	touch $@

${SSM_LIB_DOMAIN}/makebidon_1.1_linux26-x86-64: ${SSM_REPOSITORY}/makebidon_1.1_linux26-x86-64.ssm
	ssm install --clobber -d ${SSM_LIB_DOMAIN} -f ${SSM_REPOSITORY}/makebidon_1.1_linux26-x86-64.ssm
	ssm publish -d ${SSM_LIB_DOMAIN} -p makebidon_1.1_linux26-x86-64 --force
	touch $@

${SSM_REPOSITORY}/makebidon_1.1_linux26-x86-64.ssm: makebidon_1.1_linux26-x86-64 ${GIT_CACHE}/rpncomm
	tar zcf ${SSM_REPOSITORY}/makebidon_1.1_linux26-x86-64.ssm --exclude=.git makebidon_1.1_linux26-x86-64
	rm -f makebidon.done

# rpncomm
rpncomm.done: ${SSM_LIB_DOMAIN}/rpncomm_4.5.16_linux26-x86-64
	install_rpn_comm.sh ${DEFAULT_INSTALL_ARCH}
	ssm publish -d ${SSM_LIB_DOMAIN} -p rpncomm_4.5.16_linux26-x86-64 --force
	touch $@

${SSM_LIB_DOMAIN}/rpncomm_4.5.16_linux26-x86-64: ${SSM_REPOSITORY}/rpncomm_4.5.16_linux26-x86-64.ssm
	ssm install --clobber -d ${SSM_LIB_DOMAIN} -f ${SSM_REPOSITORY}/rpncomm_4.5.16_linux26-x86-64.ssm
	ssm publish -d ${SSM_LIB_DOMAIN} -p rpncomm_4.5.16_linux26-x86-64 --force
	touch $@

${SSM_REPOSITORY}/rpncomm_4.5.16_linux26-x86-64.ssm: rpncomm_4.5.16_linux26-x86-64 ${GIT_CACHE}/rpncomm
	tar zcf ${SSM_REPOSITORY}/rpncomm_4.5.16_linux26-x86-64.ssm --exclude=.git rpncomm_4.5.16_linux26-x86-64
	rm -f rpncomm.done

# vgrid descriptors
vgrid.done: ${SSM_LIB_DOMAIN}/vgrid_6.1.gnu_linux26-x86-64
	install_vgrid.sh ${DEFAULT_INSTALL_ARCH}
	ssm publish -d ${SSM_LIB_DOMAIN} -p vgrid_6.1.gnu_linux26-x86-64 --force
	touch $@

${SSM_LIB_DOMAIN}/vgrid_6.1.gnu_linux26-x86-64: ${SSM_REPOSITORY}/vgrid_6.1.gnu_linux26-x86-64.ssm
	ssm install --clobber -d ${SSM_LIB_DOMAIN} -f ${SSM_REPOSITORY}/vgrid_6.1.gnu_linux26-x86-64.ssm
	ssm publish -d ${SSM_LIB_DOMAIN} -p vgrid_6.1.gnu_linux26-x86-64 --force
	touch $@

${SSM_REPOSITORY}/vgrid_6.1.gnu_linux26-x86-64.ssm: vgrid_6.1.gnu_linux26-x86-64
	tar zcf ${SSM_REPOSITORY}/vgrid_6.1.gnu_linux26-x86-64.ssm --exclude=.git vgrid_6.1.gnu_linux26-x86-64
	rm -f vgrid.done

# afsisio_1.0u_all
${SSM_ENV_DOMAIN}/afsisio_1.0u_all: $(SSM_REPOSITORY)/afsisio_1.0u_all.ssm
	ssm install --clobber -d ${SSM_ENV_DOMAIN} -u $(SSM_REPOSITORY) -p afsisio_1.0u_all
	ssm publish -d ${SSM_ENV_DOMAIN} -p afsisio_1.0u_all --force

# armnlib_2.0u_all
${SSM_ENV_DOMAIN}/armnlib_2.0u_all: $(SSM_REPOSITORY)/armnlib_2.0u_all.ssm
	ssm install --clobber -d ${SSM_ENV_DOMAIN} -u $(SSM_REPOSITORY) -p armnlib_2.0u_all
	ssm publish -d ${SSM_ENV_DOMAIN} -p armnlib_2.0u_all --force






