export NEW_EC_ARCH=unknown
case "${COMPILER_CHOICE:-ifort}" in
   pgi | pgi-17)
           export NEW_EC_ARCH=${BASE_ARCH}/pgi
           ;;
   gfortran | gfortran-5.4)
           export NEW_EC_ARCH=${BASE_ARCH}/gfortran
           ;;
   ifort | ifort16)
           export NEW_EC_ARCH=${BASE_ARCH}/intel
           ;;
   *)
           echo "ERROR: unrecognized Compiler Architecture '${COMPILER_CHOICE}'"
           unset NEW_EC_ARCH
           return 111
           ;;
esac
#
[[ "${EC_ARCH}" == "${NEW_EC_ARCH}" ]] && echo "WARNING: ===== keeping current ${EC_ARCH} compiling architecture ====="  && unset NEW_EC_ARCH && unset COMPILER_CHOICE && true && return
#export STRICT_ARCH=${STRICT_ARCH:-WARNING}
export STRICT_ARCH=${STRICT_ARCH:-ERROR}
if [[ -n ${COMP_ARCH:-${EC_ARCH#${BASE_ARCH}}} ]] ; then
  u.banner "=${STRICT_ARCH:-ERROR}="
  echo "Compiler Architecture '${COMP_ARCH:-${EC_ARCH#${BASE_ARCH}}}' already in use, cannot switch to '${NEW_EC_ARCH##*/}'"
  u.banner '========='
  [[ "${STRICT_ARCH:-ERROR}" == *ERROR* ]] && unset NEW_EC_ARCH && return 111
fi
echo "INFO: setting compiling architecture to '${NEW_EC_ARCH}'"
#
OMP_NUM_THREADS_SAVE=${OMP_NUM_THREADS}
#
case "${COMPILER_CHOICE:-pgi}" in
   pgi | pgi-17)
           export COMP_ARCH=pgi
#           module load pgi
           which pgf90 || return 111
           pgf90 -V
           mpif90 -V
           echo "=== USING: pgi compiler ==="
           ;;
   gfortran | gfortran-5.4)
           export COMP_ARCH=gfortran
#           module load gcc
           which gfortran || return 111
           gfortran --version
           mpif90 --version
           echo "=== using gfortran/gcc ==="
           ;;
   ifort16)
           export COMP_ARCH=intel
#           module load intel
           which ifort || return 111
           ifort --version
           mpif90 --version
           echo === USING: intel compiler ===
           ;;
   *)
           echo "ERROR: unknown compiler architecture '${COMPILER_CHOICE}'"
           return 111
           ;;
esac
#
#
export OMP_NUM_THREADS=${OMP_NUM_THREADS_SAVE}
#
unset COMPILER_CHOICE
unset NEW_EC_ARCH
unset ExtraPath
unset ExtraLdPath
export EC_ARCH=${BASE_ARCH}/${COMP_ARCH}
[[ -n ${FORCE_OLD_ARCH} ]] && export EC_ARCH=${BASE_ARCH}_${COMP_ARCH}
export ARCH=${EC_ARCH}
