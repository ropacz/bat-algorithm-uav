#!/bin/bash
# Run Qtenv with fixes for macOS display issues

export OMNETPP_ROOT="/Users/rodrigo/omnetpp-workspace/omnetpp-6.2.0"
export INET_PROJ="/Users/rodrigo/omnetpp-workspace/inet-4.5.4"
export OPP_ENV_VERSION="1.0"

# Fix for Qt display issues on macOS
export QT_MAC_WANTS_LAYER=1
export QT_AUTO_SCREEN_SCALE_FACTOR=1
export QT_ENABLE_HIGHDPI_SCALING=1

# Force light theme (helps with icon visibility)
export QT_QPA_PLATFORMTHEME=""
export QT_STYLE_OVERRIDE="Fusion"

# Alternative: Force specific Qt platform
# export QT_QPA_PLATFORM=cocoa

source "$OMNETPP_ROOT/setenv" -f

cd /Users/rodrigo/omnetpp-workspace/bat-algorithm/simulations

echo "==========================================="
echo "Abrindo Qtenv com correções para macOS"
echo "==========================================="
echo ""
echo "Se os ícones ainda não aparecerem, tente:"
echo "  1. Mudar tema do macOS (claro/escuro)"
echo "  2. Usar modo express (sem animação)"
echo "  3. Usar o IDE ao invés do Qtenv"
echo ""

# Use first argument as config, default to QuickTest
CONFIG=${1:-QuickTest}

# Run with Qtenv (using RELEASE version to match INET)
../out/clang-release/src/bat-algorithm -u Qtenv -c $CONFIG \
    -n .:../src:$INET_PROJ/src \
    --image-path=$INET_PROJ/images \
    -l $INET_PROJ/src/INET \
    omnetpp.ini
