#!/bin/bash

# excute environment setup
. `dirname ${BASH_SOURCE[0]}`/envsetup.sh

# excute efl_jhbuild
efl_jhbuild

# excute efl_gyp
gyp_chromiumefl
