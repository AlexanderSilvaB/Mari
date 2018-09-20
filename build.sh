#!/bin/bash

cd src
if [ ! -d "build-$1" ]; then
    qibuild configure -c $1
fi
qibuild make -c $1
cd ..