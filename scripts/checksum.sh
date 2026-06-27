#!/bin/sh
set -eu
file=$1
cksum "$file" | awk '{print $1"  '$file'"}'
