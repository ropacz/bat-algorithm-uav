#!/bin/bash
# Script to regenerate Makefile with correct INET path

export OMNETPP_ROOT="/Users/rodrigo/omnetpp-workspace/omnetpp-6.2.0"
export INET_PROJ="/Users/rodrigo/omnetpp-workspace/inet-4.5.4"
export OPP_ENV_VERSION="1.0"

source "$OMNETPP_ROOT/setenv" -f

cd src
opp_makemake -f --deep -O out -I. -I"$INET_PROJ/src" -L"$INET_PROJ/src" -lINET -KINET4_5_PROJ="$INET_PROJ"
