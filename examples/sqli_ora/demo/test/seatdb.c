T_PkgType pre_seat_index_type[] = {		/* 母表索引 */
	{CH_CHAR,13,"Train_no",0,-1},
	{CH_CHAR,2,"item"},	/* 组号，一般＝'1' */
	{CH_JUL,sizeof(INT4),"beg_date",YEAR_TO_DAY}, /* 有效起始日期 YYYY-MM-DD */
	{CH_JUL,sizeof(INT4),"end_date",YEAR_TO_DAY},
	{CH_TINY,1,"flag"},	/* 0:有效，其它：无效*/
	{CH_TIME,sizeof(INT64),"distribute_time",YEAR_TO_SEC}, /* 最后发布时间 YYYY-MM-DD HH24:MI:SS*/
	{CH_CHAR,21,"note"},
	{-1,0,0}
};
T_PkgType pre_seat_type[] = {		/* 母表 */
	{CH_CHAR,24,"Pre_ID",0,-1},
	{CH_TINY,1,"Carno"},	/* 车厢号，不分车厢的票车厢号＝0 */
	{CH_SHORT,sizeof(short),"seat_type"},	/*席别，0＝无号 */
	{CH_SHORT,sizeof(short),"beg_seatno"},
	{CH_SHORT,sizeof(short),"end_seatno"},
	{CH_SHORT,sizeof(short),"beg_station"},	/* 上车站 */
	{CH_SHORT,sizeof(short),"end_station"},	/* 最远站 */
	{CH_SHORT,sizeof(short),"shortest_station"},	/* 限售以远 */
	{CH_INT,sizeof(int),"purpose"},	/* 用途 */
	{CH_CHAR,51,"gride"},	/* 列车等级，新空调直达特快等 */
	{-1,0,0}
};
T_PkgType seat_type[] = {		/* 席位表 */
	{CH_DATE,YEAR_TO_DAY_LEN,"start_date",YEAR_TO_DAY,-1}, /* 始发日期 YYYY-MM-DD*/
	{CH_CHAR,5,"beg_station"},	/* 上车站 */
	{CH_CHAR,13,"Train_no"},	/* 始发车次 */
	{CH_CHAR,7,"run_train"},	/* 运行车次 */
	{CH_DATE,YEAR_TO_MIN_LEN,"on_date",YEAR_TO_MIN,-1}, /* 上车时间 YYYY-MM-DD HH24:mi*/
	{CH_TINY,1,"Carno"},	/* 车厢号，不分车厢的票车厢号＝0 */
	{CH_SHORT,sizeof(short),"seat_type"},	/*席别，0＝无号*/
	{CH_SHORT,sizeof(short),"seat_no"},	/* 席位号 */
	{CH_SHORT,sizeof(short),"end_station"},	/* 售前是最远站，售后改为下车站 */
	{CH_SHORT,sizeof(short),"shortest_station"},	/* 限售以远 */
	{CH_INT,sizeof(int),"purpose"},	/* 用途 */
	{CH_CHAR,51,"gride"},   	/* 列车等级，新空调直达特快等 */
	{CH_CHAR,6,"pro"},   	/* 席位属性码，靠窗等 */
	{CH_TINY,1,"flag"},		/* 标志：0正常,1占用,2已售,-1禁售 */
	{CH_CHAR,17,"used_dev"},	/* 最后操作终端 */
	{CH_CHAR,17,"used_uid"},	/* 最后操作人 */
	{CH_TIME,sizeof(INT64),"used_time",YEAR_TO_SEC}, /* 最后操作时间 YYYY-MM-DD HH24:MI:SS */
	{CH_CHAR,20,"ROWID"},
	{-1,0,0}
};

T_PkgType seat_remant_type[] = { 	/* 存量表 */
	{CH_DATE,YEAR_TO_DAY_LEN,"on_date",YEAR_TO_DAY,-1}, /* 上车日期 */
	{CH_CHAR,5,"beg_station"},	/* 上车站 */
	{CH_CHAR,13,"run_train"},	/* 运行车次 */
	{CH_TINY,1,"Carno"},	/* 车厢号，不分车厢的票车厢号＝0 */
	{CH_SHORT,sizeof(short),"seat_type"},	/*席别，0＝无号*/
	{CH_INT,sizeof(int),"purpose"},	/* 用途 */
	{CH_SHORT,sizeof(short),"end_station"},	/* 最远站 */
	{CH_SHORT,sizeof(short),"amoung"},
	{CH_CHAR,20,"ROWID"},
	{-1,0,0}
};
	
