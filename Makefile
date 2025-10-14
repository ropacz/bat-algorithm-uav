# Makefile for bat-algorithm project
# Export required environment variables for OMNeT++
export OPP_ENV_VERSION ?= 1.0
export OMNETPP_ROOT ?= /Users/rodrigo/omnetpp-workspace/omnetpp-6.2.0
export INET_PROJ ?= /Users/rodrigo/omnetpp-workspace/inet-4.5.4

all: checkmakefiles
	cd src && $(MAKE)

clean: checkmakefiles
	cd src && $(MAKE) clean

cleanall: checkmakefiles
	cd src && $(MAKE) MODE=release clean
	cd src && $(MAKE) MODE=debug clean
	rm -f src/Makefile
	rm -rf simulations/results

makefiles:
	@echo "Generating Makefiles..."
	@if [ -z "$$INET_PROJ" ]; then \
		echo "Warning: INET_PROJ not set. Trying to find INET..."; \
		if [ -d "../inet" ]; then \
			INET_PROJ=../inet; \
		elif [ -d "../../inet" ]; then \
			INET_PROJ=../../inet; \
		else \
			echo "Error: Cannot find INET Framework."; \
			echo "Please set INET_PROJ environment variable."; \
			exit 1; \
		fi; \
	fi; \
	cd src && opp_makemake -f --deep \
		-O out \
		-KINET_PROJ=$$INET_PROJ \
		-DINET_IMPORT \
		-I. \
		-I$$INET_PROJ/src \
		-L$$INET_PROJ/src \
		-lINET\$$\(D\)

checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '======================================================================='; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '======================================================================='; \
	echo; \
	exit 1; \
	fi

run:
	cd simulations && ./run

test:
	cd simulations && ./run -c QuickTest

help:
	@echo "Bat Algorithm UAV Swarm - Build Targets:"
	@echo ""
	@echo "  make                 - Build the project"
	@echo "  make makefiles       - Generate Makefiles"
	@echo "  make clean           - Clean build files"
	@echo "  make cleanall        - Clean all build files and results"
	@echo "  make run             - Run simulation (Qtenv)"
	@echo "  make test            - Run quick test"
	@echo "  make help            - Show this help"
	@echo ""
	@echo "Before building, make sure INET_PROJ is set:"
	@echo "  export INET_PROJ=/path/to/inet"

.PHONY: all clean cleanall makefiles checkmakefiles run test help

