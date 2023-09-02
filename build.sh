#!/bin/bash

IS_CUSTOM_DIRECTORY=0

if [ $# -ne 1 ]; then
	IS_CUSTOM_DIRECTORY=1	
fi

if [ $IS_CUSTOM_DIRECTORY -eq 0 ]; then
	CUSTOM_DIRECTORY=${1}	
else
	CUSTOM_DIRECTORY=${PWD}
fi

if [ -e ${CUSTOM_DIRECTORY} ]; then
	echo "${CUSTOM_DIRECTORY}"
else
	echo "Custom directory doesn't exist"
	exit 1
fi

BUILD_DIRECTORY="build/"
echo "${BUILD_DIRECTORY}"

FULL_BUILD_DIR="${CUSTOM_DIRECTORY}/${BUILD_DIRECTORY}"
if [ -d "${FULL_BUILD_DIR}" ]; then
	echo "Deleting old build directory..."
	rm -rf ${FULL_BUILD_DIR}
fi

echo "Creating new build directory..."
mkdir ${FULL_BUILD_DIR}

cmake -B${BUILD_DIRECTORY} -S.

cmake --build ${BUILD_DIRECTORY}
