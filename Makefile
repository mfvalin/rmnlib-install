include rmnlib-install.cfg

phase1:	prerequis rmnlib-install.dot ${INSTALL_HOME}  ${SSM_REPOSITORY} ${SSM_DOMAIN_HOME}
	@printf '====================== phase 1 done ======================\n\n'
	. ${SSM_DOMAIN_HOME}/etc/ssm.d/profile && make phase2

phase2: ${SSM_BASE_DOMAIN} ${SSM_LIB_DOMAIN} ${SSM_ENV_DOMAIN} \
	${SSM_DOMAIN_HOME}/ssmuse_1.4.1_all \
	${SSM_DOMAIN_HOME}/ssm-wrappers_1.0.u_all \
	${SSM_DOMAIN_HOME}/env-setup_003_all \
	listd
	@printf '====================== phase 2 done ======================\n\n'
	make phase3

phase3: ${SSM_BASE_DOMAIN}/dot-profile-setup_2.0_all \
        ${SSM_ENV_DOMAIN}/shortcut-tools_1.0_all \
        ${SSM_ENV_DOMAIN}/cmcarc_4.3.1u_linux26-x86-64 \
        listd liste
	@printf '====================== phase 3 done ======================\n\n'

listd:
	@ssm listd -d ${SSM_DOMAIN_HOME}

liste:
	ssm listd -d ${SSM_ENV_DOMAIN}

prerequis:
	which /bin/ksh93
	which gfortran
	which git
	which perl
	which python
	for i in  File::Spec::Functions File::Basename URI::file Cwd  ; do perl -e "use 5.008_008; use strict; use $$i" ; done

${INSTALL_HOME}:
	mkdir -p $@

${SSM_REPOSITORY}:
	mkdir -p $@

${SSM_BASE_DOMAIN}:
	ssm created --verbose -d $@ --sources ${SSM_REPOSITORY}

${SSM_ENV_DOMAIN}:
	ssm created --verbose -d $@ --sources ${SSM_REPOSITORY}

${SSM_LIB_DOMAIN}:
	ssm created --verbose -d $@ --sources ${SSM_REPOSITORY}

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
${SSM_BASE_DOMAIN}/dot-profile-setup_2.0_all: ${SSM_REPOSITORY}/dot-profile-setup_2.0_all.ssm
	ssm install -d ${SSM_BASE_DOMAIN} -f ${SSM_REPOSITORY}/dot-profile-setup_2.0_all.ssm
	ssm publish -d ${SSM_BASE_DOMAIN} -p dot-profile-setup_2.0_all

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

