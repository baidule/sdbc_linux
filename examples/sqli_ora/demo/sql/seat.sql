/* default with not null */
/* 母表分为母表索引和母表两部分如下：*/

drop table pre_seat_index;
create table pre_seat_index (		-- 母表索引
	Train_no varchar2(12),	-- 全车次 
	item	char(1) default '1',-- 组号，一般＝'1' 
	beg_date date,	-- 有效起始日期 YYYY-MM-DD 
	end_date date,	-- 有效结束日期 YYYY-MM-DD 
	flag	number(1), --* 0:有效，其它：无效
	distribute_time date null, -- 最后发布时间 YYYY-MM-DD HH24:MI:SS
	note	varchar(20),
	primary key (Train_no,item,end_date) 
);
drop table pre_seat;
create table pre_seat (	--  母表 
	Pre_ID varchar2(23), -- 索引，由pre_seat_index里的字段拼凑而来：
-- Train_no:itam:beg_date(YYYYMMDD)
	Carno number(2), -- 车厢号，不分车厢的票车厢号＝0 
	seat_type number(4),  --席别，0＝无号 
	beg_seatno number(3), -- 开始席号
	end_seatno number(3),-- 结束席号
	beg_station number(3), -- 上车站 
	end_station number(3),  -- 最远站 
	shortest_station number(3),  -- 限售以远 
	purpose	number(9),		-- 用途 
	gride	varchar2(50),   -- 事由码，新空调直达特快等。
	primary key (Pre_ID,Carno,beg_seatno,beg_station)
);

drop table seat;
create table seat (		/* 席位表 */
	start_date	date,	/* 始发日期 YYYY-MM-DD*/
	beg_station varchar2(4),	/* 上车站略码 */
	Train_no varchar2(12),	/* 全车次 */
	run_train	varchar2(6),	/* 运行车次 */
	on_date	date,		/* 上车日期 YYYY-MM-DD HH24:MI*/
	Carno number(2),	/* 车厢号，不分车厢的票车厢号＝0 */
	seat_type number(2),	/*席别，0＝无号*/
	seat_no   number(3),	/* 席位号 */
	end_station number(3),	/* 顺号，售前是最远站，售后改为下车站 */
	shortest_station number(3),	/* 顺号，限售以远 */
	purpose	number(9),		/* 用途 */
	gride	varchar2(50),   -- 事由码，新空调直达特快等。
	pro	char(5) null,	-- 席位属性码（靠窗，走道等
	flag	number(1),	/* 标志：0正常,1占用,2已售,3暂不可用，-1禁售 */
	used_dev varchar2(16) null, /* 最后操作终端 */
	used_uid varchar2(16) null, /* 最后操作人 */
	used_time date  null,		/* 最后操作时间 YYYY-MM-DD HH24:MI:SS */
	primary key(start_date,beg_station,Train_no,Carno,
			seat_no,seat_type)
);
-- create index seat_idx on seat(beg_station,on_date,run_train,end_station);
create index seat_release on seat(flag,used_time); /* 释放占用 */

drop table seat_remant;
create table seat_remant ( 	/* 存量表 */
	on_date	date,			/* 上车日期 */
	beg_station varchar2(4),	/* 上车站略码 */
	run_train varchar2(12),		/* 运行车次 */
	Carno number(2),	/* 车厢号，不分车厢的票车厢号＝0 */
	seat_type number(4),	/*席别，0＝无号*/
	purpose	number(9),		/* 用途 */
	end_station number(3),	/* 最远站 */
	amoung	number(3), 	/* 数量 */
	primary key(on_date,beg_station,run_train,Carno,
		seat_type,purpose,end_station)
);
	
