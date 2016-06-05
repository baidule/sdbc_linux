#! /bin/bash
# 用 xinetd 启动
#在/etc/services里：
# sqlsrv 11055/tcp

export HOME=/home/sdbc
export SDBCDIR=$HOME/sdbc

#export DISPLAY=:0.0

#这个要改为你的环境
export ORACLE_HOME=/home/oracle/instantclient_11_2
export PATH=$ORACLE_HOME/bin:$PATH:$HOME/bin
export LD_LIBRARY_PATH=$ORACLE_HOME/lib:$ORACLE_HOME/ctx/lib:/lib:/usr/lib:/usr/local/lib:$HOME/lib
NLS_LANG=American_America.ZHS16GBK;export NLS_LANG
ORA_NLS33=$ORACLE_HOME/ocommon/nls/admin/data;export ORA_NLS33
export NLS_DATE_FORMAT='YYYY-MM-DD HH24:MI:SS'
export LANG=zh_CN

WDIR=$HOME/bin

cd $WDIR

./sqlsrv $HOME/etc/sqlsrv.cfg
