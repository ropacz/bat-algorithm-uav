#!/bin/bash
# Quick build wrapper that sets up OMNeT++ environment
export OMNETPP_ROOT="/Users/rodrigo/omnetpp-workspace/omnetpp-6.2.0"
export INET_PROJ="/Users/rodrigo/omnetpp-workspace/inet-4.5.4"
export OPP_ENV_VERSION="1.0"

source "$OMNETPP_ROOT/setenv" -f

# Default to debug mode, but allow MODE to be specified
MODE=${MODE:-debug}
cd src && make MODE=$MODE "$@"

