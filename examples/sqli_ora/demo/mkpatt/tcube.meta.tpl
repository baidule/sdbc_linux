
T_PkgType cube_tpl[]={
	{CH_CHAR,9,"unit",0,-1},
	{CH_CHAR,9,"(select tabname from $DB.dict where tabname=:tabname) tabname"},
	{CH_SHORT,sizeof(short),"flg"},
	{CH_INT,sizeof(int),"count(*) count"},
	{CH_INT64,sizeof(INT64),"sum(dat1) s"},
	{CH_DOUBLE,sizeof(double),"avg(dat1) ag","%.2lf"},
	{-1,0,"tjrb","tjdate|unit|tabname|flg|"}
};
