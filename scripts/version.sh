#!/bin/sh
set -eu
grep '#define MF_VERSION_STRING' include/markforge/common.h | sed 's/.*"\(.*\)".*/\1/'
