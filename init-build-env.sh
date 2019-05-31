#!/bin/bash
# Usage: bash init-build-env.sh BUILD-DIR
# Example: bash init-build-env.sh build

function abort {
	echo $1
	echo "Aborted"
	exit -1
}

function detect_uefi_firmware_image {
	UEFI_FIRMWARE_PATH_LIST="/usr/share/ovmf/x64/OVMF_CODE.fd /usr/share/OVMF/OVMF_CODE.fd"

	local PATH_CANDIDATE
	for PATH_CANDIDATE in ${UEFI_FIRMWARE_PATH_LIST}; do
		if [ -n "${PATH_CANDIDATE}" ]; then
			if [ -e "${PATH_CANDIDATE}" ]; then
				UEFI_FIRMWARE_IMAGE_PATH="${PATH_CANDIDATE}"
				break
			fi
		fi
	done
}

BUILD_DIR="$1"
SRC_DIR="."

if [ -e "${BUILD_DIR}" ]; then
	abort "Error: "\'"${BUILD_DIR}"\'" already exists. Remove it to continue."
fi

echo "Initializing building environment: ${BUILD_DIR}"

detect_uefi_firmware_image
if [ -v UEFI_FIRMWARE_IMAGE_PATH ]; then
	echo "Detected UEFI firmware image: ${UEFI_FIRMWARE_IMAGE_PATH}"
	UEFI_FIRMWARE_IMAGE_EXISTS="true"
else
	echo "Error: Cannot find an available UEFI firmware image"
	unset UEFI_FIRMWARE_IMAGE_EXISTS
fi

mkdir -pv ${BUILD_DIR}

MAKEFILE_PATH="${BUILD_DIR}/makefile"
echo "Generating makefile: ${MAKEFILE_PATH}"

TARGET_SWITCH_RUN="CONFIG_TARGET_SWITCH_RUN=true"
TARGET_SWITCH_DEBUG="CONFIG_TARGET_SWITCH_DEBUG=true"

if [ ! -v UEFI_FIRMWARE_IMAGE_EXISTS ]; then
	echo "Disabling 'run' and 'debug' targets for lacking UEFI firmware image"
	TARGET_SWITCH_RUN=""
	TARGET_SWITCH_DEBUG=""
fi

MAKEFILE_CONTENT="# paths to directories
PROJECT_SRC_DIR=$(realpath -e --relative-to=${BUILD_DIR} .)
PROJECT_BUILD_DIR=.

# target switches controlling whether a target exists
${TARGET_SWITCH_RUN}
${TARGET_SWITCH_DEBUG}

# external dependencies
UEFI_FIRMWARE_IMAGE_PATH = ${UEFI_FIRMWARE_IMAGE_PATH}

#include main part in source directory
include \$(PROJECT_SRC_DIR)/build.mk

"

echo "${MAKEFILE_CONTENT}" >${MAKEFILE_PATH}
if [ $? -eq 0 ]; then
	echo "Makefile generation done"
else
	abort "Error: Failed to write makefile"
fi

