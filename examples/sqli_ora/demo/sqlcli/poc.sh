#!/bin/sh
if [ $# = 0 ]
then
	NUM=1
else
	NUM=$1
fi

LOOP=0
while [ $LOOP -lt $NUM ]
do
./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &./sqlcli cli.ini 2>&1 &
	LOOP=`expr $LOOP + 1`
done
wait
