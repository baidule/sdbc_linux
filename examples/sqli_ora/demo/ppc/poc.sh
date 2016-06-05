if [ $# -eq 0 ]
then
	NUM=1
else
	NUM=$1
fi

N=1
w=`date +%w`

>$HOME/log/scli$w.log
while [ $N -le $NUM  ]
do
	./sqlcli oper.ini 100 >/dev/null &./sqlcli oper.ini 100 >/dev/null &./sqlcli dict.ini 100 >/dev/null &./sqlcli oper.ini 100 >/dev/null &./sqlcli oper.ini 100 >/dev/null &./sqlcli oper.ini 100 >/dev/null &./sqlcli oper.ini 100 >/dev/null &./sqlcli query.ini 100 >/dev/null &./sqlcli oper.ini 100 >/dev/null &./sqlcli oper.ini 100 >/dev/null &
	N=`expr $N + 1 `
done
wait
