
T_PkgType station_type[]={
	{CH_CHAR,5,"station_code",0,-1},
	{CH_CHAR,21,"station_name"},
	{CH_JUL,sizeof(INT4),"beg_date",YEAR_TO_DAY},
	{CH_JUL,sizeof(INT4),"end_date",YEAR_TO_DAY},
	{CH_CHAR,7,"abbreviation"},
	{CH_CHAR,17,"unit"},
	{CH_CHAR,9,"line"},
	{CH_CHAR,17,"area"},
	{CH_CHAR,21,"spell"},
	{CH_CHAR,3,"station_type"},
	{CH_INT,sizeof(int),"statis_code"},
	{CH_TINY,1,"flag"},
	{CH_TINY,1,"ac_fee"},
	{CH_DOUBLE,sizeof(double),"x"},
	{CH_DOUBLE,sizeof(double),"y"},
	{-1,0,0}
};

T_PkgType train_type[]={
	{CH_CHAR,13,"Train_no",0,-1},
	{CH_JUL,sizeof(INT4),"beg_date",YEAR_TO_DAY},
	{CH_JUL,sizeof(INT4),"end_date",YEAR_TO_DAY},
	{CH_CHAR,5,"start_station"},
	{CH_CHAR,5,"end_station"},
	{CH_SHORT,sizeof(short),"station_num"},
	{CH_INT,sizeof(int),"total_distance"},
	{CH_SHORT,sizeof(short),"start_time"},
	{CH_TINY,1,"iterms"},
	{CH_CHAR,32,"cycle"},
	{CH_CHAR,6,"gride"},
	{CH_INT,sizeof(int),"price_mode"},
	{CH_INT,sizeof(int),"discount_mode"},
	{CH_SHORT,sizeof(short),"days"},
	{CH_TINY,1,"flag"},
	{CH_CHAR,17,"owner"},
	{CH_CHAR,51,"note"},
	{-1,0,0}
};

T_PkgType stop_station_type[]={
	{CH_CHAR,13,"Train_no",0,-1},
	{CH_CHAR,7,"run_Train_no"},
	{CH_SHORT,sizeof(short),"sequence"},
	{CH_JUL,sizeof(INT4),"beg_date",YEAR_TO_DAY},
	{CH_JUL,sizeof(INT4),"end_date",YEAR_TO_DAY},
	{CH_CHAR,5,"station_code"},
	{CH_INT,sizeof(int),"distance"},
	{CH_INT,sizeof(int),"running_time"},
	{CH_TINY,1,"stop_time"},
	{CH_SHORT,sizeof(short),"over"},
	{CH_TINY,1,"flag"},
	{-1,0,0}
};
	
