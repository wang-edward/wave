#!/bin/bash

BIN_PATH="./build/unit_tests --verbose=1"

cmake -DCMAKE_BUILD_TYPE=Debug -B build

if [[ $? == 0 ]]; then
    make -C build -j6
else
    echo "cmake failed"
    exit 1
fi

if [[ $? == 0 ]]; then
    $BIN_PATH
else
    echo "make failed"
    exit 1
fi
