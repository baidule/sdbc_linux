if [ $# = 0 ]
then
	FILE=PERM.TXT
else
	FILE=$1
fi

awk -F'[ ]*[|][ ]*' -f perm_rule.awk $FILE
