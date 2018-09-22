#!/bin/bash

robot=$1

echo "Sync keys"
mkdir -p keys
if [ ! -f ~/.ssh/id_rsa ]; then
    ssh-keygen -t rsa -f ~/.ssh/id_rsa -q -P ""
fi

SSH_CMD="ssh -o ControlMaster=auto -o ControlPath=/tmp/control_%l_%h_%p_%r"

cat ~/.ssh/id_rsa.pub | $SSH_CMD -l nao $robot "mkdir -p .ssh; sh -c 'cat >> .ssh/authorized_keys'; chmod 700 .ssh; chmod 600 .ssh/authorized_keys "

$SSH_CMD -t -l nao $robot "su -c 'tee /etc/sudoers <<< \"nao     ALL=(ALL) NOPASSWD:ALL\" && chmod 0440 /etc/sudoers && chown root:root /etc/sudoers'"

echo "Placing files"
cp src/build-robo/sdk/bin/rinobot root/home/nao/
cp src/build-robo/sdk/lib/libagent.so root/home/nao/naoqi/preferences/libagent.so

echo "Sync root"
rsync -aburvP --rsync-path="sudo rsync" root/etc nao@$robot:/
echo "Sync home"
rsync -aurvP root/home/nao/ nao@$robot:~/

echo "Turning off services we do not use"
# Add back in then delete because delete throws error if not installed
#  * Networking connection manager replaced with runswiftwireless above
$SSH_CMD nao@$robot "sudo rc-update add connman boot"
$SSH_CMD nao@$robot "sudo rc-update del connman boot"
$SSH_CMD nao@$robot "sudo rc-update add qimessaging-json default"
$SSH_CMD nao@$robot "sudo rc-update del qimessaging-json default"

$SSH_CMD nao@$robot "nao restart"