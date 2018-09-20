#!/bin/bash

cd src
if [ ! -d "buils-robo" ]; then
    qibuild configure -c robo
fi
qibuild make -c robo
cd ..