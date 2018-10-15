#!/bin/bash

configure=false

for i in "$@" ; do
    if [[ $i == "-configure" ]] ; then
        configure=true
    fi
done

if [ ! -d "src/build-$1" ]; then
    configure=true
fi

cd src
if [ $configure == true ]; then
    qibuild configure -c $1
fi
qibuild make -c $1
cd ..
