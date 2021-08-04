#!/usr/bin/bash
for ii in {0..80}
do
ifconfig ens192 down 
ifconfig ens160 down 
ifconfig ens224 down 
sleep 5
ifconfig ens192 up 
ifconfig ens160 up 
ifconfig ens224 up 
sleep 5
done
