SHELL = bash

SSM_VERBOSE = 

default: phase4.done

include rmnlib-install.cfg

# phase 1 : create package repository, create installation master directory, install ssm
phase1: 
	make phase1.done

phase1.done: dependencies.done rmnlib-install.dot ${INSTALL_HOME}  ${SSM_REPOSITORY} ${SSM_DOMAIN_HOME}
	@printf '====================== phase 1 done ======================\n\n'
	touch phase1.done

# phase 2 : needs ssm
#           create environment domain
#           install and publish ssmuse, ssm wrappers, user profile setup, setup utilities
#                               cmcarc, shortcuts, environment utilities, compiling tools
phase2: phase1.done
	. ${SSM_DOMAIN_HOME}/etc/ssm.d/profile && make phase2.done

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
	${SSM_ENV_DOMAIN}/armnlib_1.0u_all \
	listd
	@printf '====================== phase 2 done ======================\n\n'
	touch phase2.done

# phase 3 : needs ssm, tools installed in phases 2 and 3, and a user setup (compilers and tools)
#           create domain for libraries, install and optionally compile libraries
phase3: phase2.done
	. ${SSM_DOMAIN_HOME}/etc/ssm.d/profile && make phase3.done

phase3.done: ${SSM_LIB_DOMAIN} \
	${SSM_LIB_DOMAIN}/massvp4_1.0_linux26-x86-64 \
	${SSM_LIB_DOMAIN}/rmnlib_016.3_linux26-x86-64 \
	${SSM_LIB_DOMAIN}/rpncomm_4.5.16_linux26-x86-64 \
        listd liste
	@printf '====================== phase 3 done ======================\n\n'
	touch phase3.done

# phase 4 : needs ssm, tools and libraries installed in phases 2 and 3, and a user setup (compilers and tools)
phase4: phase4.done
	. ${SSM_DOMAIN_HOME}/etc/ssm.d/profile ; . env-setup.dot ; make phase4.done

phase4.done: ${SSM_LIB_DOMAIN}
	@printf '====================== phase 4 done ======================\n\n'
#	touch phase4.done

wipe_install:
	mkdir -p ${INSTALL_HOME}   && rm -rf ${INSTALL_HOME}
	mkdir -p ${SSM_REPOSITORY} && rm -rf ${SSM_REPOSITORY}
	rm -f *.done
listd:
	@ssm listd -d ${SSM_DOMAIN_HOME}

liste:
	ssm listd -d ${SSM_ENV_DOMAIN}

dependencies.done:
	@which /bin/ksh       || echo "ERROR: /bin/ksh not found"
	@which /bin/ksh93     || echo "ERROR: /bin/ksh93 not found"
	@echo "typeset -A aa; aa['tagada']='shimboum' ; typeset -Z4 n" | /bin/ksh93  || echo "ERROR: not a bona fide ksh93"
	@which gfortran       || echo "ERROR: gfortran not found"
	@which git            || echo "ERROR: git not found"
	@which python         || echo "ERROR: python not found"
	@which perl           || echo "ERROR: perl not found"
	@for i in  File::Spec::Functions File::Basename URI::file Cwd  ; do \
	    perl -e "use 5.008_008; use strict; use $$i" ; \
	    done  ||  echo "ERROR: missing needed perl modules"
	touch dependencies.done

mpidependencies.done:
	@printf "program mpi\ncall mpi_init(ierr)\nprint *,'Hello World'\ncall mpi_finalize(ierr)\nstop\nend\n" >mpitest.F90
	@mpif90 -o mpitest.Abs mpitest.F90
	@mpirun -n 2 ./mpitest.Abs
	@rm -f mpitest.F90 mpitest.Abs
	touch mpidependencies.done

${INSTALL_HOME}:
	mkdir -p $@

${SSM_REPOSITORY}:
	mkdir -p $@

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
	    git clone ${GIT_HOME}/ssm_fork.git ssm_10.151_all && \
	    tar zcf ssm_10.151_all.ssm --exclude=.git ssm_10.151_all

${SSM_DOMAIN_HOME}: ${SSM_REPOSITORY}/ssm_10.151_all.ssm
	${SSM_REPOSITORY}/ssm_10.151_all/bin/ssm-installer_10.151_all.sh \
	    --label 'ssm package' --force \
	    --domainHome ${SSM_DOMAIN_HOME} \
	    --ssmRepositoryUrl ${SSM_REPOSITORY} \
	    --defaultRepositorySource ${SSM_REPOSITORY}

# ssmuse_1.4.1_all
${SSM_DOMAIN_HOME}/ssmuse_1.4.1_all: ${SSM_REPOSITORY}/ssmuse_1.4.1_all.ssm
	ssm install -d ${SSM_DOMAIN_HOME} -f ${SSM_REPOSITORY}/ssmuse_1.4.1_all.ssm
	ssm publish -d ${SSM_DOMAIN_HOME} -p ssmuse_1.4.1_all

${SSM_REPOSITORY}/ssmuse_1.4.1_all.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_HOME}/ssmuse_fork.git ssmuse_1.4.1_all && \
	    tar zcf ssmuse_1.4.1_all.ssm --exclude=.git ssmuse_1.4.1_all

# ssm-wrappers_1.0.u_all
${SSM_DOMAIN_HOME}/ssm-wrappers_1.0.u_all: ${SSM_REPOSITORY}/ssm-wrappers_1.0.u_all.ssm
	ssm install -d ${SSM_DOMAIN_HOME} -f ${SSM_REPOSITORY}/ssm-wrappers_1.0.u_all.ssm
	ssm publish -d ${SSM_DOMAIN_HOME} -p ssm-wrappers_1.0.u_all

${SSM_REPOSITORY}/ssm-wrappers_1.0.u_all.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_HOME}/ssm-wrappers ssm-wrappers_1.0.u_all && \
	    tar zcf ssm-wrappers_1.0.u_all.ssm --exclude=.git ssm-wrappers_1.0.u_all

#env-setup_003_all
${SSM_DOMAIN_HOME}/env-setup_003_all: ${SSM_REPOSITORY}/env-setup_003_all.ssm
	ssm install -d ${SSM_DOMAIN_HOME} -f ${SSM_REPOSITORY}/env-setup_003_all.ssm
	ssm publish -d ${SSM_DOMAIN_HOME} -p env-setup_003_all

${SSM_REPOSITORY}/env-setup_003_all.ssm:
	tar zcf ${SSM_REPOSITORY}/env-setup_003_all.ssm env-setup_003_all

# dot-profile-setup_2.0_all
${SSM_DOMAIN_HOME}/dot-profile-setup_2.0_all: ${SSM_REPOSITORY}/dot-profile-setup_2.0_all.ssm
	ssm install -d ${SSM_DOMAIN_HOME} -f ${SSM_REPOSITORY}/dot-profile-setup_2.0_all.ssm
	ssm publish -d ${SSM_DOMAIN_HOME} -p dot-profile-setup_2.0_all

${SSM_REPOSITORY}/dot-profile-setup_2.0_all.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_HOME}/dot-profile-setup dot-profile-setup_2.0_all && \
	    tar zcf dot-profile-setup_2.0_all.ssm --exclude=.git dot-profile-setup_2.0_all

#shortcut-tools_1.0_all
${SSM_ENV_DOMAIN}/shortcut-tools_1.0_all: ${SSM_REPOSITORY}/shortcut-tools_1.0_all.ssm
	ssm install -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/shortcut-tools_1.0_all.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p shortcut-tools_1.0_all

${SSM_REPOSITORY}/shortcut-tools_1.0_all.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_HOME}/shortcut-tools shortcut-tools_1.0_all && \
	    tar zcf shortcut-tools_1.0_all.ssm --exclude=.git shortcut-tools_1.0_all

#cmcarc_4.3.1u_linux26-x86-64
${SSM_ENV_DOMAIN}/cmcarc_4.3.1u_linux26-x86-64: ${SSM_REPOSITORY}/cmcarc_4.3.1u_linux26-x86-64.ssm
	ssm install -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/cmcarc_4.3.1u_linux26-x86-64.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p cmcarc_4.3.1u_linux26-x86-64

${SSM_REPOSITORY}/cmcarc_4.3.1u_linux26-x86-64.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_HOME}/cmcarc_fork cmcarc_4.3.1u_linux26-x86-64 && \
	    tar zcf cmcarc_4.3.1u_linux26-x86-64.ssm --exclude=.git cmcarc_4.3.1u_linux26-x86-64

#env-utils_1.0u_all
${SSM_ENV_DOMAIN}/env-utils_1.0u_all: ${SSM_REPOSITORY}/env-utils_1.0u_all.ssm
	ssm install -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/env-utils_1.0u_all.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p env-utils_1.0u_all

${SSM_REPOSITORY}/env-utils_1.0u_all.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_HOME}/env-utils env-utils_1.0u_all && \
	    tar zcf env-utils_1.0u_all.ssm --exclude=.git env-utils_1.0u_all

#code-tools_1.0_all
${SSM_ENV_DOMAIN}/code-tools_1.0_all: ${SSM_REPOSITORY}/code-tools_1.0_all.ssm
	ssm install -d ${SSM_ENV_DOMAIN} -f ${SSM_REPOSITORY}/code-tools_1.0_all.ssm
	ssm publish -d ${SSM_ENV_DOMAIN} -p code-tools_1.0_all

${SSM_REPOSITORY}/code-tools_1.0_all.ssm:
	cd ${SSM_REPOSITORY} && \
	    git clone ${GIT_HOME}/code-tools code-tools_1.0_all && \
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
${SSM_ENV_DOMAIN}/afsisio_1.0u_all:
	ssm install --skipOnInstalled -d ${SSM_ENV_DOMAIN} -u $(REMOTE_URL) -p afsisio_1.0u_all
	ssm publish -d ${SSM_ENV_DOMAIN} -p afsisio_1.0u_all --force

# armnlib_1.0u_all
${SSM_ENV_DOMAIN}/armnlib_1.0u_all:
	ssm install --skipOnInstalled -d ${SSM_ENV_DOMAIN} -u $(REMOTE_URL) -p armnlib_1.0u_all
	ssm publish -d ${SSM_ENV_DOMAIN} -p armnlib_1.0u_all --force






