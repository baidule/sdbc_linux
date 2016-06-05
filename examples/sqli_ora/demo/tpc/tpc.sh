#!/bin/sh
#usage: nohup tpc.sh &
#

while true
do
	tpcsrv ~/etc/tpcsrv.cfg
	sleep 10
done

