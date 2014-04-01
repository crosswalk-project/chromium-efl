#!/bin/bash

pushd `dirname ${BASH_SOURCE[0]}` > /dev/null
SCRIPT_PATH=`pwd -L`
popd > /dev/null

PROJECT_ROOT_PATH="${SCRIPT_PATH}/.."
if [[ -z "${CHROME_SRC}" ]]; then
  # If $CHROME_SRC was not set, assume src subfolder in project directory is CHROME_SRC.
  export CHROME_SRC="${PROJECT_ROOT_PATH}/src"
fi

function applyPatch {
  directory=$1
  shift
  patchfile=$1
  shift
  echo Applying $patchfile in $directory
  pushd "$directory" > /dev/null
  patch -p1 $@ < "$patchfile"
  popd > /dev/null
}

