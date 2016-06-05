
T_PkgType TRAIN_PRICE_tpl[]={
	{CH_INT,sizeof(int),"id",0,-1},
	{CH_CHAR,4,"start_station"},
	{CH_CHAR,4,"end_station"},
	{CH_DATE,YEAR_TO_DAY_LEN,"beg_date",YEAR_TO_DAY},
	{CH_DATE,YEAR_TO_DAY_LEN,"end_date",YEAR_TO_DAY},
	{CH_INT,sizeof(int),"yz_fee"},
	{CH_INT,sizeof(int),"rz_fee"},
	{CH_INT,sizeof(int),"rz1_fee"},
	{CH_INT,sizeof(int),"rz2_fee"},
	{CH_INT,sizeof(int),"yws_fee"},
	{CH_INT,sizeof(int),"ywz_fee"},
	{CH_INT,sizeof(int),"ywx_fee"},
	{CH_INT,sizeof(int),"rws_fee"},
	{CH_INT,sizeof(int),"rwx_fee"},
	{CH_INT,sizeof(int),"g2s_fee"},
	{CH_INT,sizeof(int),"g2x_fee"},
	{CH_CHAR,51,"note"},
	{-1,0,"TRAIN_PRICE","id|start_station|end_station|end_date|"}
};
