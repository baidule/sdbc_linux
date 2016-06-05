#生成JAVA用模板包 (模板名.json,模板名.java)
#usage: mkpkg.sh 模板名
#$1 模板名.txt :逐行列出本包的表名

mkpatt.sh -J $1 < $1.txt
