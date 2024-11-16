#!/bin/bash

SIZE=${1:-100}
ITERATIONS=${2:-100}
TYPE=${3:-0}
EXECUTABLE=${5:-cmake-build-debug/solutions/serial/serial_solution}
OUTPUT_DIR=${6:-cmake-build-debug/solutions/serial/snapshots}

./$EXECUTABLE $SIZE $ITERATIONS $TYPE $OUTPUT_DIR
