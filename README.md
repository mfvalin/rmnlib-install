rmnlib-install provides a portable version of the core RPN environment.

# Setup
Until something better is devised, here is a quick and dirty setup recipe.

## Requirements

* Clean environment: deactivate `.bashrc`, `.profile`, etc.

* Use `/bin/bash` as login shell.

* A working `gfortran` and `opemnmpi` are expected to be available. These packages should be available in standard distributions.

## Steps

```bash
git clone https://github.com/mfvalin/rmnlib-install
mv rmnlib-install/Makefile.bootstrap Makefile
cp rmnlib-install/default-install.cfg rmnlib-install/rmnlib-install.cfg
adjust rmnlib-install/rmnlib-install.cfg
export LANG=C
make
```

You will be prompted to create some directories and maybe to install some software.

Perform adjustments/installs and rerun make. This may be an iterative process.

## Usage

The installer will create a file:

`${HOME}/.profile_armnlib`

You can source it with this command:

`. ${HOME}/.profile_armnlib`

To activate the environment.
