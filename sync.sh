#!/bin/bash

if [ "$#" -le 1 ]; then
    echo "Invalid number of arguments"
    echo "Usage: ./sync.sh robot-hostname toolchain-name"
    exit
fi

robot=$1
toolchain=$2
sudo=false
ssh=false
root=false
services=false
home=true
restart=false
build=false
configure=false
uninstall=false
clear=false
pwd="nao"
user="nao"

nextIsPwd=false
nextIsUser=false
for i in "$@" ; do 
    if [ $nextIsPwd == true ] ; then
	pwd=$i
	nextIsPwd=false
    elif [ $nextIsUser == true ] ; then
	user=$i
	nextIsUser=false
    elif [[ $i == "-pwd" ]] ; then
        nextIsPwd=true
    elif [[ $i == "-user" ]] ; then
        nextIsUser=true
    elif [[ $i == "-sudo" ]] ; then
        sudo=true
    elif [[ $i == "-clear" ]] ; then
        clear=true
    elif [[ $i == "-ssh" ]] ; then
        ssh=true
    elif [[ $i == "-root" ]] ; then
        root=true
    elif [[ $i == "-services" ]] ; then
        services=true
    elif [[ $i == "-not-home" ]] ; then
        home=false
    elif [[ $i == "-restart" ]] ; then
        restart=true
    elif [[ $i == "-build" ]] ; then
        build=true
    elif [[ $i == "-uninstall" ]] ; then
        uninstall=true
    elif [[ $i == "-configure" ]] ; then
        configure=true
    elif [[ $i == "-initial" ]] ; then
        configure=true
        build=true
        sudo=true
        ssh=true
        root=true
        services=true
        home=true
        restart=true
        configure=true
    elif [[ $i == "-all" ]] ; then
        build=true
        sudo=true
        ssh=true
        root=true
        services=true
        home=true
        restart=true
        configure=true
    fi
done

SSH_CMD="sshpass -p $pwd ssh -o ControlMaster=auto -o ControlPath=/tmp/control_%l_%h_%p_%r"

if [ $clear == true ]; then
    echo "Cleaning $toolchain"
    rm -rf src/build-$toolchain
    exit
fi

if [ $uninstall == true ]; then
    echo "Uninstalling from $robot"
    exit
fi

if [ $build == true ]; then
    if [ $configure == true ]; then
        ./build.sh $2 -configure
    else
        ./build.sh $2
    fi
fi

if [ $ssh == true ]; then
    echo "Sync keys"
    mkdir -p keys
    if [ ! -f ~/.ssh/id_rsa ]; then
        ssh-keygen -t rsa -f ~/.ssh/id_rsa -q -P ""
    fi

    #cat ~/.ssh/id_rsa.pub | $SSH_CMD -l nao $robot "mkdir -p .ssh; sh -c 'cat >> .ssh/authorized_keys'; chmod 700 ~/.ssh; chmod 600 ~/.ssh/authorized_keys ; chmod g-w,o-w /home/nao"
    ssh-copy-id $user@$robot
fi

if [ $sudo == true ]; then
    echo "Disable sudo"
    $SSH_CMD -t -l nao $robot "su -c 'tee /etc/sudoers <<< \"nao     ALL=(ALL) NOPASSWD:ALL\" && chmod 0440 /etc/sudoers && chown root:root /etc/sudoers'"
fi

echo "Placing files"
cp src/build-$toolchain/sdk/bin/rinobot root/home/nao/
cp src/build-$toolchain/sdk/lib/libagent.so root/home/nao/naoqi/preferences/libagent.so

if [ $root == true ]; then
    echo "Sync root"
    sshpass -p $pwd rsync -aburvP --rsync-path="sudo rsync" root/etc $user@$robot:/
fi

if [ $home == true ]; then
    echo "Sync home"
    sshpass -p $pwd rsync -aurvP root/home/nao/ $user@$robot:~/naoqi/
fi

if [ $services == true ]; then
    echo "Turning off services we do not use"
    # Add back in then delete because delete throws error if not installed
    #  * Networking connection manager replaced with runswiftwireless above
    $SSH_CMD $user@$robot "sudo rc-update add connman boot"
    $SSH_CMD $user@$robot "sudo rc-update del connman boot"
    $SSH_CMD $user@$robot "sudo rc-update add qimessaging-json default"
    $SSH_CMD $user@$robot "sudo rc-update del qimessaging-json default"
    echo "Turning on services needed"
    $SSH_CMD $user@$robot "sudo rc-update add ntp-client default"
fi

if [ $restart == true ]; then
    echo "Restart Naoqi"
    $SSH_CMD $user@$robot "sudo /etc/init.d/naoqi restart"
fi
