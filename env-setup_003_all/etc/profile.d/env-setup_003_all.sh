export ENV_RELEASE=${ENV_RELEASE:-003}
[[ -d ${SSM_DOMAIN_HOME%/*}/base_${ENV_RELEASE} ]] && . ssmuse-sh -d ${SSM_DOMAIN_HOME%/*}/base_${ENV_RELEASE}
. .profile_00000
#
[[ -d ${SSM_DOMAIN_HOME%/*}/env_${ENV_RELEASE} ]] && . ssmuse-sh -d ${SSM_DOMAIN_HOME%/*}/env_${ENV_RELEASE}
#
export HOME_ECssm=$(s.unified_setup-cfg -base)
HOME_ECssm=${HOME_ECssm%/dot-profile-setup*}
s.which local_shortcuts.dot 2>/dev/null 1>/dev/null && . local_shortcuts.dot
#
. .profile_zzzzz
