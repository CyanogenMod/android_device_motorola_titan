#!/bin/sh

set -e

export VENDOR=motorola
export DEVICE=titan
./../../$VENDOR/msm8226-common/extract-files.sh $@
