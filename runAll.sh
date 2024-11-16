#!/bin/bash

SIZE=${1:-100}
ITERATIONS=${2:-100}
TYPE=${3:-0}
NUM_PROCS=${4:-4}

rm serial.webm async.webm async_block.webm &> /dev/null

rm cmake-build-debug/solutions/serial/snapshots/* &> /dev/null
rm cmake-build-debug/solutions/async/snapshots/* &> /dev/null
rm cmake-build-debug/solutions/async_block/snapshots/* &> /dev/null

./scripts/async.sh $SIZE $ITERATIONS $TYPE $NUM_PROCS
just video cmake-build-debug/solutions/async/snapshots/ async
rm cmake-build-debug/solutions/async/snapshots/* &

./scripts/async_block.sh $SIZE $ITERATIONS $TYPE $NUM_PROCS
just video cmake-build-debug/solutions/async_block/snapshots/ async_block
rm cmake-build-debug/solutions/async_block/snapshots/* &

./scripts/serial.sh $SIZE $ITERATIONS $TYPE $NUM_PROCS
just video cmake-build-debug/solutions/serial/snapshots/ serial
rm cmake-build-debug/solutions/serial/snapshots/*
