#!/bin/sh

set -e

export DEVICE=titan
export VENDOR=motorola
./../../$VENDOR/msm8226-common/setup-makefiles.sh $@
