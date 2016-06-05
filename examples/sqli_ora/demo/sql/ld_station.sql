
/* default with not null 
-- drop table station;
create table station (
	station_code varchar2(4),
	station_name varchar2(20),
	beg_date     date,
	abbreviation varchar2(6) null,
	unit	     varchar2(16) null,
	line	     varchar2(8) null,
	area	     varchar2(16) null,
	spell	     varchar2(20),
	station_type varchar2(2) null,
	statis_code  number(6) null,
	flag	     number(1),
	ac_money     number(2),         -- 车站空调费 
	x	     number(8,3) null,
	y	     number(8,3) null
);
*/
insert into station values('BNHP','北京南',to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),' ','01010000','','','bjn','KY',101001,0,1,0,0);
insert into station values('YZHP','亦庄',to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),' ','01010000','','','yz','KY',101002,0,1,0,0);
insert into station values('YLHP','永乐',to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),' ','01010000','','','lf','KY',101003,0,0,0,0);
insert into station values('WQHP','武清',to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),' ','01010000','','','wq','XC',101004,1,0,0,0);
insert into station values('TJHP','天津',to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),' ','01010000','','','tj','KY',101005,0,1,0,0);
insert into station values('TSHP','唐山',to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),' ','01010000','','','ts','KY',101006,0,0,0,0);
insert into station values('BHHP','北戴河',to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),' ','01010000','','','bdh','KY',101007,0,1,0,0);
insert into station values('QHHP','秦皇岛',to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),' ','01010000','','','qhd','KY',101008,0,0,0,0);
insert into station values('SJHP','石家庄',to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),' ','01010000','','','sjz','KY',101009,0,1,0,0);
insert into station values('BDHP','保定',to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),' ','01010000','','','bd','KY',101010,0,0,0,0);

-- create table Train (	/* 车次表 */
-- 	Train_no	varchar2(20),
-- 	beg_date	date,
-- 	start_station	varchar2(4), /* ?? */
-- 	end_station	varchar2(4),
-- 	station_num	number(3),
-- 	total_distance	number(6),
-- 	start_time	number(4),
-- 	xw_iterm  		number(2),       /* 计算席位周期 */
-- 	xw_cycle		varchar2(31),
-- 	gride		char(5), /* 等级，普、快、特快等 */
-- 	price_mode	number(6),
--	discount_mode   number(6),
-- 	days		number(3) null, /*  席位发行天数 */
-- 	flag		number(1),	 /* 0可用，1不可用 */
-- 	owner		varchar2(16) null
-- 	note		varchar2(50) null
-- ) ;
insert into Train values('H1',to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BNHP','TJHP',6,130,495,1,'1','G04',0,0,null,0,'01010000',null);
insert into Train values('H2',to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'TJHP','BNHP',6,130,570,1,'1','G04',0,0,null,0,'01010000',null);
insert into Train values('H4',to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'SJHP','QHHP',7,590,1430,1,'01','G04',0,0,null,0,'01010000',null);
insert into Train values('H4A',to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'SJHP','BHHP',6,540,1430,1,'10','G04',0,0,null,0,'01010000',null);
insert into Train values('H4',to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'SJHP','QHHP',7,590,780,1,'01','G04',0,0,30,0,'01010000',null);
insert into Train values('H4A',to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'SJHP','BHHP',6,540,780,1,'10','G04',0,0,30,0,'01010000',null);
insert into Train values('H6A',to_date('2008-04-03','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BHHP','SJHP',6,540,10,1,'10','G04',0,0,30,0,'01010000','H6');
insert into Train values('H6B',to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'QHHP','SJHP',7,590,1435,1,'10','G04',0,0,30,0,'01010000','H6');

-- create table stop_station (
-- 	Train_no	varchar2(20),  /* 始发车次 */
-- 	run_Train_no	varchar2(20),  /* 运行车次 */
-- 	item		char(1) null,  /* 运行周期 暂不使用*/
-- 	sequence	number(3),		/* 顺号 */
-- 	beg_date	date,		/* 有效起始日期 */
--      end_date        date,           /* 有效结束日期 */
-- 	station_code	varchar2(8),	/* 车站代码 */
-- 	distance	number(6),		/* 自始发站里程 */
-- 	running_time	number(6),		/* 自始发站运行分钟数 */
-- 	stop_time	number(2),		/* 停靠分钟数 */
-- 	over		number(4) null,	/* 超员数(本站可售无号数) */		
-- 	flag		number(1)		/* 0可用，1封站 */
-- );
	
insert into stop_station values('H1','H1',0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BNHP',0,0,0,0,0);
insert into stop_station values('H1','H1',1,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'YZHP',30,10,2,30,0);
insert into stop_station values('H1','H1',2,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'YLHP',60,20,2,30,0);
insert into stop_station values('H1','H1',4,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'WQHP',90,31,2,0,0);
insert into stop_station values('H1','H1',5,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'TJHP',130,45,0,0,0);
insert into stop_station values('H2','H2',0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'TJHP',0,0,0,0,0);
insert into stop_station values('H2','H2',1,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'WQHP',40,14,2,0,0);
insert into stop_station values('H2','H2',2,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'YLHP',70,25,2,30,0);
insert into stop_station values('H2','H2',4,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'YZHP',100,35,2,30,0);
insert into stop_station values('H2','H2',5,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BNHP',130,45,0,0,0);
insert into stop_station values('H4','H4',0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'SJHP',0,0,0,0,0);
insert into stop_station values('H4','H4',1,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'BDHP',150,30,2,0,0);
insert into stop_station values('H4','H5',2,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'BNHP',270,57,5,0,0);
insert into stop_station values('H4','H5',3,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'TJHP',400,88,3,40,0);
insert into stop_station values('H4','H5',4,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'TSHP',470,106,1,40,0);
insert into stop_station values('H4','H5',5,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'BHHP',540,122,5,40,0);
insert into stop_station values('H4','H5',6,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'QHHP',590,137,0,0,0);
insert into stop_station values('H4A','H4',0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'SJHP',0,0,0,0,0);
insert into stop_station values('H4A','H4',1,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'BDHP',150,30,2,0,0);
insert into stop_station values('H4A','H5',2,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'BNHP',270,57,5,0,0);
insert into stop_station values('H4A','H5',3,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'TJHP',400,88,3,40,0);
insert into stop_station values('H4A','H5',4,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'TSHP',470,106,1,40,0);
insert into stop_station values('H4A','H5',5,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),'BHHP',540,122,0,0,0);
	
insert into stop_station values('H6B','H6',0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'QHHP',0,0,0,0,0);
insert into stop_station values('H6B','H6',1,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BHHP',50,10,5,0,0);
insert into stop_station values('H6B','H6',2,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'TSHP',120,30,1,0,0);
insert into stop_station values('H6B','H6',3,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'TJHP',190,46,3,0,0);
insert into stop_station values('H6B','H3',4,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BNHP',320,75,3,0,0);
insert into stop_station values('H6B','H3',5,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BDHP',440,103,1,40,0);
insert into stop_station values('H6B','H3',6,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'SJHP',590,134,5,0,0);
insert into stop_station values('H6A','H6',0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BHHP',0,0,0,0,0);
insert into stop_station values('H6A','H6',1,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'TSHP',70,15,1,0,0);
insert into stop_station values('H6A','H6',2,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'TJHP',140,31,3,0,0);
insert into stop_station values('H6A','H3',3,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BNHP',270,59,3,0,0);
insert into stop_station values('H6A','H3',4,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BDHP',390,87,1,40,0);
insert into stop_station values('H6A','H3',5,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'SJHP',540,118,5,0,0);

insert into stop_station values('H4','H4',0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'SJHP',0,0,0,0,0);
insert into stop_station values('H4','H4',1,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BDHP',150,30,2,0,0);
insert into stop_station values('H4','H5',2,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BNHP',270,57,5,0,0);
insert into stop_station values('H4','H5',3,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'TJHP',400,88,3,40,0);
insert into stop_station values('H4','H5',4,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'TSHP',470,106,1,40,0);
insert into stop_station values('H4','H5',5,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BHHP',540,122,5,40,0);
insert into stop_station values('H4','H5',6,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'QHHP',590,137,0,0,0);
insert into stop_station values('H4A','H4',0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'SJHP',0,0,0,0,0);
insert into stop_station values('H4A','H4',1,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BDHP',150,30,2,0,0);
insert into stop_station values('H4A','H5',2,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BNHP',270,57,5,0,0);
insert into stop_station values('H4A','H5',3,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'TJHP',400,88,3,40,0);
insert into stop_station values('H4A','H5',4,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'TSHP',470,106,1,40,0);
insert into stop_station values('H4A','H5',5,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),'BHHP',540,122,0,0,0);
