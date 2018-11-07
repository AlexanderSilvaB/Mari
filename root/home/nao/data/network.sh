#!/bin/bash

team=47
robot=01
network=true
ip=false
server="10.0.0.100"
mask="255.255.0.0"

nextIsNumber=false
nextIsTeam=false
nextIsServer=false
nextIsMask=false
for i in "$@" ; do 
    if [ $nextIsNumber == true ] ; then
        robot=$i
	    nextIsNumber=false
    elif [ $nextIsTeam == true ] ; then
        team=$i
	    nextIsTeam=false
    elif [ $nextIsServer == true ] ; then
        server=$i
	    nextIsServer=false
    elif [ $nextIsMask == true ] ; then
        mask=$i
	    nextIsMask=false
    elif [[ $i == "-no-network" ]] ; then
        network=false
    elif [[ $i == "-set-ip" ]] ; then
        ip=true
    elif [[ $i == "-robot" ]] ; then
        nextIsNumber=true
    elif [[ $i == "-team" ]] ; then
        nextIsTeam=true
    elif [[ $i == "-mask" ]] ; then
        nextIsMask=true
    elif [[ $i == "-server" ]] ; then
        nextIsServer=true
    fi
done

IP="10.0.$team.$robot"

if [ $network == true ]; then
    echo "Setting netwrk profile"
    killall -q wpa_supplicant
    ifconfig wlan0 down
    sleep 2
    killall -q wpa_supplicant
    ifconfig wlan0 up
    wpa_supplicant -iwlan0 -Dnl80211 -c/home/nao/data/network/default -B
fi

if [ $ip == true ]; then
    echo "Setting IP = $IP"
    echo "Setting netmask = $mask"
    echo "Setting default gateway = $server"
    ifconfig wlan0 "$IP" netmask "$mask"
    route add -net default gw "$server"
else
    echo "Setting DHCP"
    killall -q dhclient
    dhclient -1 wlan0
fi

echo "Finished"