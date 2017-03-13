#!/usr/bin/env bash
# Script to copy and rename compiled static library

DST_ROOT="$1"

# Find library file in current directory
LIB_PATH=${PWD}
FILE=$(find lib*.a)

# 1. Get the substring to be removed
REMOVED="${FILE#*_}"
REMOVED="_${REMOVED%.*}"

# 2. Target file name
TARGET="${FILE/${REMOVED}/}"   # Replace ${REMOVED} with nothing

# 3. To lower case
LOWER=$(echo "${TARGET}" | tr '[A-Z]' '[a-z]')

# 4. Copy to destination path
cp ${FILE} ${DST_ROOT}/${LOWER}