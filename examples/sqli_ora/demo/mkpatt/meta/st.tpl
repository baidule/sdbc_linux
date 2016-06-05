
T_PkgType st_tpl[]={
	{CH_TINY,1,"c",0,-1},
	{CH_INT64,sizeof(INT64),"bi"},
	{-1,0,0,0}
};

T_PkgType ss_tpl[]={
	{CH_TINY,1,"c",0,-1},
	{CH_STRUCT,0,"s",(const char *)st_tpl},
	{CH_INT,sizeof(int),"i"},
	{-1,0,0,0}
};
