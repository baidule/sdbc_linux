
T_PkgType testmeta_tpl[]={
	{CH_TINY,1,"NC0012 id",0,-1},
	{CH_CHAR,17,"NG0013 devid"},
	{CH_CHAR,17,"NU0001 userid"},
	{CH_TIME,sizeof(INT64),"TI0104 logtime",YEAR_TO_SEC},
	{CH_JUL,sizeof(INT4),"TI0105 beg_date",YEAR_TO_DAY},
	{CH_INT,sizeof(int),"count(*) cnt"},
	{CH_CLOB,sizeof(char *),"CL0001 cmd"},
	{CH_CNUM,35,"NC0015 quantity"},
	{-1,0,"TUXTICKET","devid|userid|"}
};
