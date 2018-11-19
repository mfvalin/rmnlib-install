# rmnlib-install

quick and dirty recipe (pending something better)

"suggested" install method :

no environment (deactivate .bashrc, .profile , ..., /bin/bash as login
shell)

git clone https://github.com/mfvalin/rmnlib-install

mv rmnlib-install/Makefile.bootstrap Makefile

cp  rmnlib-install/default-install.cfg rmnlib-install/rmnlib-install.cfg
adjust rmnlib-install/rmnlib-install.cfg

make

you will be prompted to create some directories and maybe to install
some software.

perform adjustments/installs and rerun make

it will be an iterative process

a working gfortran and opemnmpi are expected to be available in the
"vanilla" environment (proper packages are available for usual distros)



