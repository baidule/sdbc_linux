/* default with not null */

drop table STATION;
create table STATION (
	station_code char(3), 	/* 电报略吗 */
	station_name varchar2(12),
	beg_date     date, /* 有效起始日 YYYY-MM-DD */
	end_date     date, /* 有效截止日 YYYY-MM-DD */
	abbreviation varchar2(2) null, /* 简称 */
	unit	     varchar2(16) null, 	/* 所属局 */
	line	     varchar2(8) null,		/* 所属线,如果是接算站或接轨站<0查接算站表看接算哪条线 */
	area	     varchar2(16) null,  /* 所在城市 */
	spell	     varchar2(20), 		/* 拼音码,小写 */
	station_type char(2) null,	/* 类型，
	[0]=@非,=A客运站,=B乘降所;[1]=@非,=A、接算站,=B接轨站,=D客运专线,=H地方铁路,=P合资铁路等 */
	statis_code  number(10) null,	/* 统计码 */
	flag	     number(1),		/* 0:有效，其它无效 */
	ac_fee     number(2),		/* 车站空调费 */
	x	     number(8,3) ,	/* 经度 */
	y	     number(8,3), 	/* 纬度 */
	primary key (station_code,end_date)
);
create index station_py on station(spell);
create index station_name on station(station_name);

drop table TRAIN;
create table TRAIN (	/* 车次表 */
	train_no	varchar2(12),	/* 全车次 */
	beg_date	date,		/* 有效起始日 YYYY-MM-DD */
	end_date	date,		/* 有效截止日 YYYY-MM-DD */
	start_station	varchar2(12),	/* 始发站  */
	end_station	varchar2(12),	/* 终到站 */
	station_num	number(3),	/* 停站数 */
	total_distance	number(6),	/* 总里程 */
	start_time	number(4),	/* 发车时间，分钟数 */
	iterms	 	number(2),      /* 席位组数 */
	cycle		varchar2(31),	/* 计算席位周期 */
	gride		char(5), 	    /* 等级，普、快、特快等 */
	seat_type	varchar(10) null,	/*具有的席别：A:硬座,B:软座,C:二等软座,D:一等软座,E:硬卧,I:软卧,L:软高包 ,O:动车二等软座 ,P:动车一等软座,S:动车软卧,T:动车硬卧 */
	price_mode	number(6),	    /* 票价规则 */
	discount_mode	number(6),	    /* 折扣规则 */
	days		number(3) null, /*  席位发行天数 */
	flag		number(1),	/* 0可用，1不可用 */
	owner		varchar2(16) null,  /* 担当单位 */
	note		varchar2(50) null,		/* 备注  */
	primary key (train_no,end_date)
);

drop table STOP_STATION;
create table STOP_STATION (
	train_no	varchar2(12),  /*  全车次 */
	run_Train_no	varchar2(6),  /* 运行车次 */
	sequence	number(3),		/* 顺号 */
	beg_date	date,		/* 有效起始日期 YYYY-MM-DD*/
	end_date	date,		/* 有效结束日期 YYYY-MM-DD*/
	station_code	varchar2(4),	/* 车站代码 */
	distance	number(6),		/* 自始发站里程 */
	running_time	number(6),		/* 自始发站运行分钟数 */
	stop_time	number(3),		/* 停靠分钟数 */
	line	number(4),			/* 所属线号 */
	area_flg	number(1),		/* 0:不是地区，1：是地区 */
	over		number(4) null,	/* 超员数(本站可售无号数) */		
	flag		number(1),		/* 0可用，1封站 */
	primary key  (train_no,sequence,end_date )
);
create index s_station_q on stop_station (Train_no,station_code,end_date);
create index stop_station_idx on stop_station(station_code,run_Train_no,end_date);
	
	
