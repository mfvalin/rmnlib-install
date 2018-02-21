SHELL = bash

SSM_VERBOSE = 

default: phase4

include rmnlib-install.cfg

##############################################################################################################
# phase 0 : populate the git cache and the ssm cache,  create package repository
##############################################################################################################
phase0: ${SSM_CACHE} ${GIT_CACHE} ${INSTALL_HOME} \
	${GIT_CACHE}/ssm_fork.git \
	${GIT_CACHE}/ssmuse_fork.git \
	${GIT_CACHE}/ssm-wrappers.git \
	${GIT_CACHE}/shortcut-tools \
	${GIT_CACHE}/cmcarc_fork \
	${GIT_CACHE}/env-utils \
	${GIT_CACHE}/code-tools \
	${GIT_CACHE}/librmn \
	${GIT_CACHE}/rpncomm \
	${GIT_CACHE}/dot-profile-setup \
	${SSM_REPOSITORY} \
	${SSM_CACHE}/afsisio_1.0u_all.ssm \
	${SSM_CACHE}/armnlib_2.0u_all.ssm
# 	touch phase0

##############################################################################################################
# phase 1 : create installation master directory, install ssm
##############################################################################################################
phase1: | phase0
	make phase1.done
	touch phase1

phase1.done: dependencies.done rmnlib-install.dot ${INSTALL_HOME} ${SSM_DOMAIN_HOME}
	@printf '====================== phase 1 done ======================\n\n'
	touch phase1.done

##############################################################################################################
# phase 2 : needs ssm
#           acquire ssm, create environment domain
#           install and publish ssmuse, ssm wrappers, user profile setup, setup utilities
#                               cmcarc, shortcuts, environment utilities, compiling tools
##############################################################################################################
phase2: | phase1
	. ${SSM_DOMAIN_HOME}/etc/ssm.d/profile && \
	  make phase2.done
	touch phase2

# packages armnlib(data+include) and afsisio to be added here
phase2.done: ${SSM_ENV_DOMAIN} \
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
	${SSM_ENV_DOMAIN}/${SSM_SHORTCUTS} \
	listd
	@printf '====================== phase 2 done ======================\n\n'
	touch phase2.done

##############################################################################################################
# phase 3 : needs ssm, tools installed in phase 2, and a user setup (compilers and tools)
#           create domain for libraries, install and optionally compile libraries
##############################################################################################################
phase3: | phase2
	. ${SSM_DOMAIN_HOME}/etc/ssm.d/profile && \
	  . env-setup.dot && \
	  export GIT_CACHE=${GIT_CACHE} && \
	  make phase3.done
	touch phase3

phase3.done: ${SSM_LIB_DOMAIN} \
	${SSM_LIB_DOMAIN}/massvp4_1.0_linux26-x86-64 \
	${SSM_LIB_DOMAIN}/rmnlib_016.3_linux26-x86-64 \
	${SSM_LIB_DOMAIN}/rpncomm_4.5.16_linux26-x86-64 \
        listd liste
	@printf '====================== phase 3 done ======================\n\n'
	touch phase3.done

##############################################################################################################
# phase 4 : needs ssm, tools and library sources installed in phases 2 and 3, 
#           and a user setup (compilers and tools)
#           compile and publish libraries
##############################################################################################################
phase4: | phase3 mpidependencies.done
	. ${SSM_DOMAIN_HOME}/etc/ssm.d/profile && \
	  . env-setup.dot && \
	  . r.load.dot ${SSM_LIB_DOMAIN} && \
	  make phase4.done
	touch phase4

phase4.done:
	install_massvp4.sh ${DEFAULT_INSTALL_ARCH}
	ssm publish -d ${SSM_LIB_DOMAIN} -p massvp4_1.0_linux26-x86-64 --force
	install_rmnlib_016.sh ${DEFAULT_INSTALL_ARCH}
	ssm publish -d ${SSM_LIB_DOMAIN} -p rmnlib_016.3_linux26-x86-64 --force
	install_rpn_comm.sh ${DEFAULT_INSTALL_ARCH}
	ssm publish -d ${SSM_LIB_DOMAIN} -p rpncomm_4.5.16_linux26-x86-64 --force
	@printf '====================== phase 4 done ======================\n\n'
	touch phase4.done

##############################################################################################################

wipe_install:
	mkdir -p ${INSTALL_HOME}   && rm -rf ${INSTALL_HOME}
	mkdir -p ${SSM_REPOSITORY} && rm -rf ${SSM_REPOSITORY}
	rm -f *.done phase?
listd:
	@ssm listd -d ${SSM_DOMAIN_HOME}

liste:
	ssm listd -d ${SSM_ENV_DOMAIN}

${GIT_CACHE}:
	@echo "PLS create directory $@ (need ~50MBytes)" ; false

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
	@which /bin/ksh       || { echo "ERROR: /bin/ksh not found" ; exit 1 ; }
	@which /bin/ksh93     || { echo "ERROR: /bin/ksh93 not found" ; exit 1 ; }
	@echo "typeset -A aa; aa['tagada']='shimboum' ; typeset -Z4 n" | /bin/ksh93  || { echo "ERROR: not a bona fide ksh93" ; exit 1 ; }
	@which gfortran       || { echo "ERROR: gfortran not found" ; exit 1 ; }
	@which git            || { echo "ERROR: git not found" ; exit 1 ; }
	@which python         || { echo "ERROR: python not found" ; exit 1 ; }
	@which perl           || { echo "ERROR: perl not found" ; exit 1 ; }
	@for i in  File::Spec::Functions File::Basename URI::file Cwd  ; do \
	    perl -e "use 5.008_008; use strict; use $$i" ||  { echo "ERROR: missing needed perl module $$i" ; exit 1 ; } \
	    done 
	touch dependencies.done

mpidependencies.done:
	@printf "program mpi\ncall mpi_init(ierr)\nprint *,'Hello World'\ncall mpi_finalize(ierr)\nstop\nend\n" >mpitest.F90
	@mpif90 -o mpitest.Abs mpitest.F90
	@mpirun -n 2 ./mpitest.Abs
	@rm -f mpitest.F90 mpitest.Abs
	touch mpidependencies.done

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

${SSM_REPOSITORY}/ssm_10.151_all.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_CACHE}/ssm_fork.git ssm_10.151_all && \
	    tar zcf ssm_10.151_all.ssm --exclude=.git ssm_10.151_all

${SSM_DOMAIN_HOME}: ${SSM_REPOSITORY}/ssm_10.151_all.ssm
	${SSM_REPOSITORY}/ssm_10.151_all/bin/ssm-installer_10.151_all.sh \
	    --label 'ssm package' --force \
	    --domainHome ${SSM_DOMAIN_HOME} \
	    --ssmRepositoryUrl ${SSM_REPOSITORY} \
	    --defaultRepositorySource ${SSM_REPOSITORY}

# ssmuse_1.4.1_all
${SSM_DOMAIN_HOME}/ssmuse_1.4.1_all: ${SSM_REPOSITORY}/ssmuse_1.4.1_all.ssm
	ssm install --skipOnInstalled -d ${SSM_DOMAIN_HOME} -f ${SSM_REPOSITORY}/ssmuse_1.4.1_all.ssm
	ssm publish -d ${SSM_DOMAIN_HOME} -p ssmuse_1.4.1_all

${SSM_REPOSITORY}/ssmuse_1.4.1_all.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_CACHE}/ssmuse_fork.git ssmuse_1.4.1_all && \
	    tar zcf ssmuse_1.4.1_all.ssm --exclude=.git ssmuse_1.4.1_all

# ssm-wrappers_1.0.u_all
${SSM_DOMAIN_HOME}/ssm-wrappers_1.0.u_all: ${SSM_REPOSITORY}/ssm-wrappers_1.0.u_all.ssm
	ssm install --skipOnInstalled -d ${SSM_DOMAIN_HOME} -f ${SSM_REPOSITORY}/ssm-wrappers_1.0.u_all.ssm
	ssm publish -d ${SSM_DOMAIN_HOME} -p ssm-wrappers_1.0.u_all

${SSM_REPOSITORY}/ssm-wrappers_1.0.u_all.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_CACHE}/ssm-wrappers ssm-wrappers_1.0.u_all && \
	    tar zcf ssm-wrappers_1.0.u_all.ssm --exclude=.git ssm-wrappers_1.0.u_all

#env-setup_003_all
${SSM_DOMAIN_HOME}/env-setup_003_all: ${SSM_REPOSITORY}/env-setup_003_all.ssm
	ssm install --skipOnInstalled -d ${SSM_DOMAIN_HOME} -f ${SSM_REPOSITORY}/env-setup_003_all.ssm
	ssm publish -d ${SSM_DOMAIN_HOME} -p env-setup_003_all

${SSM_REPOSITORY}/env-setup_003_all.ssm:
	tar zcf ${SSM_REPOSITORY}/env-setup_003_all.ssm env-setup_003_all

# dot-profile-setup_2.0_all
${SSM_DOMAIN_HOME}/dot-profile-setup_2.0_all: ${SSM_REPOSITORY}/dot-profile-setup_2.0_all.ssm
	ssm install --skipOnInstalled -d ${SSM_DOMAIN_HOME} -f ${SSM_REPOSITORY}/dot-profile-setup_2.0_all.ssm
	ssm publish -d ${SSM_DOMAIN_HOME} -p dot-profile-setup_2.0_all

${SSM_REPOSITORY}/dot-profile-setup_2.0_all.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_CACHE}/dot-profile-setup dot-profile-setup_2.0_all && \
	    tar zcf dot-profile-setup_2.0_all.ssm --exclude=.git dot-profile-setup_2.0_all

#shortcut-tools_1.0_all
${SSM_ENV_DOMAIN}/shortcut-tools_1.0_all: ${SSM_REPOSITORY}/shortcut-tools_1.0_all.ssm
	ssm install --skipOnInstalled -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/shortcut-tools_1.0_all.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p shortcut-tools_1.0_all

${SSM_REPOSITORY}/shortcut-tools_1.0_all.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_CACHE}/shortcut-tools shortcut-tools_1.0_all && \
	    tar zcf shortcut-tools_1.0_all.ssm --exclude=.git shortcut-tools_1.0_all

# shortcuts (generic or site specific)
${SSM_ENV_DOMAIN}/${SSM_SHORTCUTS}: ${SSM_REPOSITORY}/${SSM_SHORTCUTS}.ssm
	ssm install --skipOnInstalled -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/${SSM_SHORTCUTS}.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p ${SSM_SHORTCUTS}

${SSM_REPOSITORY}/${SSM_SHORTCUTS}.ssm:
	tar zcf ${SSM_REPOSITORY}/${SSM_SHORTCUTS}.ssm ${SSM_SHORTCUTS}

#cmcarc_4.3.1u_linux26-x86-64
${SSM_ENV_DOMAIN}/cmcarc_4.3.1u_linux26-x86-64: ${SSM_REPOSITORY}/cmcarc_4.3.1u_linux26-x86-64.ssm
	ssm install --skipOnInstalled -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/cmcarc_4.3.1u_linux26-x86-64.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p cmcarc_4.3.1u_linux26-x86-64

${SSM_REPOSITORY}/cmcarc_4.3.1u_linux26-x86-64.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_CACHE}/cmcarc_fork cmcarc_4.3.1u_linux26-x86-64 && \
	    tar zcf cmcarc_4.3.1u_linux26-x86-64.ssm --exclude=.git cmcarc_4.3.1u_linux26-x86-64

#env-utils_1.0u_all
${SSM_ENV_DOMAIN}/env-utils_1.0u_all: ${SSM_REPOSITORY}/env-utils_1.0u_all.ssm
	ssm install --skipOnInstalled -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/env-utils_1.0u_all.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p env-utils_1.0u_all

${SSM_REPOSITORY}/env-utils_1.0u_all.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_CACHE}/env-utils env-utils_1.0u_all && \
	    tar zcf env-utils_1.0u_all.ssm --exclude=.git env-utils_1.0u_all

#code-tools_1.0_all
${SSM_ENV_DOMAIN}/code-tools_1.0_all: ${SSM_REPOSITORY}/code-tools_1.0_all.ssm
	ssm install --skipOnInstalled -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/code-tools_1.0_all.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p code-tools_1.0_all

${SSM_REPOSITORY}/code-tools_1.0_all.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_CACHE}/code-tools code-tools_1.0_all && \
	    tar zcf code-tools_1.0_all.ssm --exclude=.git code-tools_1.0_all

# r.gppf_1.0.1_linux26-x86-64
${SSM_ENV_DOMAIN}/r.gppf_1.0.1_linux26-x86-64: ${SSM_REPOSITORY}/r.gppf_1.0.1_linux26-x86-64.ssm
	ssm install --skipOnInstalled -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/r.gppf_1.0.1_linux26-x86-64.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p r.gppf_1.0.1_linux26-x86-64 --force

${SSM_REPOSITORY}/r.gppf_1.0.1_linux26-x86-64.ssm:
	tar zcf ${SSM_REPOSITORY}/r.gppf_1.0.1_linux26-x86-64.ssm --exclude=.git r.gppf_1.0.1_linux26-x86-64

# massvp4_1.0_linux26-x86-64
${SSM_LIB_DOMAIN}/massvp4_1.0_linux26-x86-64: ${SSM_REPOSITORY}/massvp4_1.0_linux26-x86-64.ssm
	ssm install --skipOnInstalled -d ${SSM_LIB_DOMAIN} -f ${SSM_REPOSITORY}/massvp4_1.0_linux26-x86-64.ssm
	ssm publish -d ${SSM_LIB_DOMAIN} -p massvp4_1.0_linux26-x86-64 --force

${SSM_REPOSITORY}/massvp4_1.0_linux26-x86-64.ssm:
	tar zcf ${SSM_REPOSITORY}/massvp4_1.0_linux26-x86-64.ssm --exclude=.git massvp4_1.0_linux26-x86-64

# rmnlib_016.3_linux26-x86-64
${SSM_LIB_DOMAIN}/rmnlib_016.3_linux26-x86-64: ${SSM_REPOSITORY}/rmnlib_016.3_linux26-x86-64.ssm
	ssm install --skipOnInstalled -d ${SSM_LIB_DOMAIN} -f ${SSM_REPOSITORY}/rmnlib_016.3_linux26-x86-64.ssm
	ssm publish -d ${SSM_LIB_DOMAIN} -p rmnlib_016.3_linux26-x86-64 --force

${SSM_REPOSITORY}/rmnlib_016.3_linux26-x86-64.ssm:
	tar zcf ${SSM_REPOSITORY}/rmnlib_016.3_linux26-x86-64.ssm --exclude=.git rmnlib_016.3_linux26-x86-64

# rpncomm_4.5.16_linux26-x86-64
${SSM_LIB_DOMAIN}/rpncomm_4.5.16_linux26-x86-64: ${SSM_REPOSITORY}/rpncomm_4.5.16_linux26-x86-64.ssm
	ssm install --skipOnInstalled -d ${SSM_LIB_DOMAIN} -f ${SSM_REPOSITORY}/rpncomm_4.5.16_linux26-x86-64.ssm
	ssm publish -d ${SSM_LIB_DOMAIN} -p rpncomm_4.5.16_linux26-x86-64 --force

${SSM_REPOSITORY}/rpncomm_4.5.16_linux26-x86-64.ssm:
	tar zcf ${SSM_REPOSITORY}/rpncomm_4.5.16_linux26-x86-64.ssm --exclude=.git rpncomm_4.5.16_linux26-x86-64

# afsisio_1.0u_all
${SSM_ENV_DOMAIN}/afsisio_1.0u_all: $(SSM_REPOSITORY)/afsisio_1.0u_all.ssm
	ssm install --skipOnInstalled -d ${SSM_ENV_DOMAIN} -u $(SSM_REPOSITORY) -p afsisio_1.0u_all
	ssm publish -d ${SSM_ENV_DOMAIN} -p afsisio_1.0u_all --force

# armnlib_2.0u_all
${SSM_ENV_DOMAIN}/armnlib_2.0u_all: $(SSM_REPOSITORY)/armnlib_2.0u_all.ssm
	ssm install --skipOnInstalled -d ${SSM_ENV_DOMAIN} -u $(SSM_REPOSITORY) -p armnlib_2.0u_all
	ssm publish -d ${SSM_ENV_DOMAIN} -p armnlib_2.0u_all --force






