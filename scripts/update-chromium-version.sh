#!/bin/bash

_SCRIPT=$(readlink -f $0)
_DIR=$(dirname $_SCRIPT)
_TOP_DIR="$_DIR/.."
_CHROMIUM_DIR="$_TOP_DIR/src"

source $_CHROMIUM_DIR/chrome/VERSION

_VERSION_BASE=$MAJOR.$BUILD.$PATCH
_VERSION_FULL=$MAJOR.$MINOR.$BUILD.$PATCH

sed -i -e "s#\(%define ChromiumVersion\ \)[[:digit:].]*#\1${_VERSION_BASE}#" $_TOP_DIR/packaging/chromium-efl.spec
printf "Chromium upsteam version: $_VERSION_FULL\n" > $_TOP_DIR/VERSION
