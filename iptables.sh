#!/bin/sh
echo "Stopping firewall and allowing everyone..."
iptables="/usr/sbin/iptables"
$iptables -F
$iptables -X
$iptables -t nat -F
$iptables -t nat -X
$iptables -t mangle -F
$iptables -t mangle -X
$iptables -P INPUT ACCEPT
$iptables -P FORWARD ACCEPT
$iptables -P OUTPUT ACCEPT