if [ $# -eq 0 ]
then
	NUM=1
else
	NUM=$1
fi

N=1

while [ $N -le $NUM  ]
do
	./kppoc kp.ini &
	N=`expr $N + 1 `
done
wait
