#!/bin/bash

echo "Rinobot CodeRelease Setup"
echo ""
echo "Installing required packages"
sudo apt-get install libnss-mdns avahi-discover avahi-utils sshpass unzip

toolchain="robot"
if [ "$#" -gt 0 ]; then
    toolchain=$1
fi

echo ""
echo "Installing SDK's"
if [ ! -d "sdk" ]; then
    echo "Creating SDK Folder"
    echo "Place a SDK file (ctc-linuxXX-atom-X.X.X.XX.zip) inside the sdk folder and run this script again" 
else
    count=$(ls -l sdk/ | grep .zip | egrep -c '^-')
    if [ $count = 0 ]; then
        echo "No SDK found"
        echo "Place a SDK file (ctc-linuxXX-atom-X.X.X.XX.zip) inside the sdk folder and run this script again" 
        exit 1
    elif [ $count -gt 1 ]; then
        echo "More than one SDK found"
        exit 1
    else
        selected=$(ls -1 sdk/ | grep .zip)
        echo "SDK Found: $selected"
        echo "Extracting"
        unzip -qq -n sdk/$selected -d sdk/
        echo "Creating toolchain $toolchain"
        filename="${selected%.*}"
        qitoolchain create $toolchain sdk/$filename/toolchain.xml
        echo "Adding config"
        qibuild add-config $toolchain -t $toolchain
        echo "Installed"
    fi
fi
