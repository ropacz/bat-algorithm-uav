#!/bin/bash
# Run simulation with proper environment

export OMNETPP_ROOT="/Users/rodrigo/omnetpp-workspace/omnetpp-6.2.0"
export INET_PROJ="/Users/rodrigo/omnetpp-workspace/inet-4.5.4"
export OPP_ENV_VERSION="1.0"

source "$OMNETPP_ROOT/setenv" -f

cd /Users/rodrigo/omnetpp-workspace/bat-algorithm/simulations

# Use first argument as config, default to QuickTest
CONFIG=${1:-QuickTest}

../out/clang-release/src/bat-algorithm -u Cmdenv -c $CONFIG \
    -n .:../src:$INET_PROJ/src \
    --image-path=$INET_PROJ/images \
    -l $INET_PROJ/src/INET \
    omnetpp.ini
