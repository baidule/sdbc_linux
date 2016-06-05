
T_PkgType PRE_SEAT_INDEX_tpl[]={
	{CH_CHAR,13,"train_no",0,-1},
	{CH_CHAR,2,"item"},
	{CH_JUL,sizeof(INT4),"beg_date",YEAR_TO_DAY},
	{CH_JUL,sizeof(INT4),"end_date",YEAR_TO_DAY},
	{CH_TINY,1,"flag"},
	{CH_TIME,sizeof(INT64),"distribute_time",YEAR_TO_SEC},
	{CH_CHAR,21,"note"},
	{-1,0,"PRE_SEAT_INDEX","train_no|item|end_date|"}
};

T_PkgType PRE_SEAT_tpl[]={
	{CH_CHAR,24,"pre_id",0,-1},
	{CH_TINY,1,"carno"},
	{CH_SHORT,sizeof(short),"seat_type"},
	{CH_SHORT,sizeof(short),"beg_seatno"},
	{CH_SHORT,sizeof(short),"end_seatno"},
	{CH_SHORT,sizeof(short),"beg_station"},
	{CH_SHORT,sizeof(short),"end_station"},
	{CH_SHORT,sizeof(short),"shortest_station"},
	{CH_INT,sizeof(int),"purpose"},
	{CH_CHAR,51,"gride"},
	{-1,0,"PRE_SEAT","pre_id|carno|beg_seatno|beg_station|"}
};

T_PkgType SEAT_tpl[]={
	{CH_DATE,YEAR_TO_DAY_LEN,"start_date",YEAR_TO_DAY,-1},
	{CH_CHAR,5,"beg_station"},
	{CH_CHAR,13,"train_no"},
	{CH_CHAR,7,"run_train"},
	{CH_DATE,YEAR_TO_MIN_LEN,"on_date",YEAR_TO_MIN},
	{CH_TINY,1,"carno"},
	{CH_TINY,1,"seat_type"},
	{CH_SHORT,sizeof(short),"seat_no"},
	{CH_SHORT,sizeof(short),"end_station"},
	{CH_SHORT,sizeof(short),"shortest_station"},
	{CH_INT,sizeof(int),"purpose"},
	{CH_CHAR,51,"gride"},
	{CH_CHAR,6,"pro"},
	{CH_TINY,1,"flag"},
	{CH_CHAR,17,"used_dev"},
	{CH_CHAR,17,"used_uid"},
	{CH_TIME,sizeof(INT64),"used_time",YEAR_TO_SEC},
	{CH_CHAR,20,"ROWID"},
	{-1,0,"SEAT","start_date|beg_station|train_no|carno|seat_no|seat_type|"}
};

T_PkgType SEAT_REMANT_tpl[]={
	{CH_DATE,YEAR_TO_DAY_LEN,"on_date",YEAR_TO_DAY,-1},
	{CH_CHAR,5,"beg_station"},
	{CH_CHAR,13,"run_train"},
	{CH_TINY,1,"carno"},
	{CH_SHORT,sizeof(short),"seat_type"},
	{CH_INT,sizeof(int),"purpose"},
	{CH_SHORT,sizeof(short),"end_station"},
	{CH_SHORT,sizeof(short),"amoung"},
	{-1,0,"SEAT_REMANT","on_date|beg_station|run_train|carno|seat_type|purpose|end_station|"}
};
