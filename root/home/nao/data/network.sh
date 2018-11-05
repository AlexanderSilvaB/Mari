#!/bin/bash

team=47
robot=01
network=true
ip=false

nextIsNumber=false
nextIsTeam=false
for i in "$@" ; do 
    if [ $nextIsNumber == true ] ; then
        robot=$i
	    nextIsNumber=false
    elif [ $nextIsTeam == true ] ; then
        team=$i
	    nextIsTeam=false
    elif [[ $i == "-no-network" ]] ; then
        network=false
    elif [[ $i == "-set-ip" ]] ; then
        ip=true
    elif [[ $i == "-robot" ]] ; then
        nextIsNumber=true
    elif [[ $i == "-team" ]] ; then
        nextIsTeam=true
    fi
done

IP="10.0.$team.$robot"

if [ $network == true ]; then
    killall -q wpa_supplicant
    ifconfig wlan0 down
    sleep 2
    killall -q wpa_supplicant
    ifconfig wlan0 up
    wpa_supplicant -iwlan0 -Dnl80211 -c/home/nao/data/network/default -B
fi

if [ $ip == true ]; then
    ifconfig wlan0 "$IP" netmask 255.255.0.0
    route add -net default gw "10.0.47.1"
else
    killall -q dhclient
    dhclient -1 wlan0
fi
