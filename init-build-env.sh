#!/bin/bash
# Usage: bash init-build-env.sh BUILD-DIR
# Example: bash init-build-env.sh build

function abort() {
	echo "Aborted."
	exit $1
}

BUILD_DIR="$1"
SRC_DIR="."

echo "Initializing building environment: ${BUILD_DIR}"

if [ -e "$1" ]; then
	echo "Error: "\'"${BUILD_DIR}"\'" already exists. Remove it to continue."
	abort -1
else
	mkdir -pv ${BUILD_DIR}
fi

MAKEFILE_PATH="${BUILD_DIR}/makefile"
echo "Generating makefile: ${MAKEFILE_PATH}"

MAKEFILE_CONTENT="# variables set when initializing building environment.
PROJECT_SRC_DIR=$(realpath -e --relative-to=${BUILD_DIR} .)
PROJECT_BUILD_DIR=.

include \$(PROJECT_SRC_DIR)/build.mk

"

echo "${MAKEFILE_CONTENT}" >${MAKEFILE_PATH}

