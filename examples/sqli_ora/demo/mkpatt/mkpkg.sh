#生成模板包 (模板名.tpl，模板名.stu)
#usage: mkpkg.sh 模板名
#$1 模板名.txt :逐行列出本包的表名

mkpatt.sh $1 < $1.txt
