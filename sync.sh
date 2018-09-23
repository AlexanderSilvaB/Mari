#!/bin/bash

if [ "$#" -le 1 ]; then
    echo "Invalid number of arguments"
    echo "Usage: ./sync.sh robot-hostname toolchain-name"
    exit
fi

SSH_CMD="ssh -o ControlMaster=auto -o ControlPath=/tmp/control_%l_%h_%p_%r"
robot=$1
toolchain=$2
sudo=false
ssh=false
root=false
services=false
home=true
restart=false

for i in "$@" ; do
    if [[ $i == "-sudo" ]] ; then
        sudo=true
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
    fi
done

if [ $ssh == true ]; then
    echo "Sync keys"
    mkdir -p keys
    if [ ! -f ~/.ssh/id_rsa ]; then
        ssh-keygen -t rsa -f ~/.ssh/id_rsa -q -P ""
    fi

    cat ~/.ssh/id_rsa.pub | $SSH_CMD -l nao $robot "mkdir -p .ssh; sh -c 'cat >> .ssh/authorized_keys'; chmod 700 ~/.ssh; chmod 600 ~/.ssh/authorized_keys ; chmod g-w,o-w /home/nao"
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
    rsync -aburvP --rsync-path="sudo rsync" root/etc nao@$robot:/
fi

if [ $home == true ]; then
    echo "Sync home"
    rsync -aurvP root/home/nao/ nao@$robot:~/
fi

if [ $services == true ]; then
    echo "Turning off services we do not use"
    # Add back in then delete because delete throws error if not installed
    #  * Networking connection manager replaced with runswiftwireless above
    $SSH_CMD nao@$robot "sudo rc-update add connman boot"
    $SSH_CMD nao@$robot "sudo rc-update del connman boot"
    $SSH_CMD nao@$robot "sudo rc-update add qimessaging-json default"
    $SSH_CMD nao@$robot "sudo rc-update del qimessaging-json default"
fi

if [ $restart == true ]; then
    echo "Restart Naoqi"
    $SSH_CMD nao@$robot "sudo /etc/init.d/naoqi restart"
fi