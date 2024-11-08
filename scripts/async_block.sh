#!/bin/bash

SIZE=${1:-100}
ITERATIONS=${2:-100}
TYPE=${3:-0}       # Default to LINE
NUM_PROCS=${4:-4}
EXECUTABLE=${5:-cmake-build-debug/solutions/async_block/async_block_solution}
OUTPUT_DIR=${6:-cmake-build-debug/solutions/async_block/snapshots}

# Run the MPI command
mpirun -n $NUM_PROCS -v $EXECUTABLE $SIZE $ITERATIONS $TYPE $OUTPUT_DIR
