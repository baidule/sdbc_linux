#!/bin/sh
#usage: nohup tpool.sh &
#

while true
do
	tpoolsrv ~/etc/tpoolsrv.cfg
	sleep 10
done

