#!/bin/bash

team=47
robot=01
network=true
ip=false
stop=false
restart=false

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
    elif [[ $i == "-stop" ]] ; then
        stop=true
    elif [[ $i == "-restart" ]] ; then
        stop=true
        network=true
    fi
done

IP="10.0.$team.$robot"

if [ $stop == true ]; then
    killall wpa_supplicant
    ifconfig wlan0 down
    sleep 2
fi

if [ $network == true ]; then
    ifconfig wlan0 up
    wpa_supplicant -iwlan0 -Dnl80211 -c /home/nao/data/network/default -B
    sleep 1
fi

if [ $ip == true ]; then
    ifconfig wlan0 "$IP" netmask 255.255.0.0
    sysctl -w net.ipv6.conf.all.disable_ipv6=1
else
    echo "iface wlan0 inet dhcp" >> /etc/network/interfaces
    ifconfig wlan0 up
    #ifconfig wlan0 0.0.0.0 0.0.0.0 && dhclient  
fi
