/ * default with not null */
/* 又一个方案：母表分为母表索引和母表两部分如下：

drop table pre_seat_index;
create table pre_seat_index (		-- 母表索引
	Train_no varchar2(6),	-- 全车次 
	item	char(1) default '1',-- 组号，一般＝'1' 
	beg_date date,	-- 有效起始日期 YYYY-MM-DD 
	end_date date,	-- 有效结束日期 YYYY-MM-DD 
	distribute_time date null, -- 最后发布时间 YYYY-MM-DD HH24:MI:SS
	flag    number(1), --* 0:有效，其它：无效
	note	varchar(20),
	flag	number(1), --* 0:有效，其它：无效
	primary key (Train_no,item,end_date) 
);
*/
insert into pre_seat_index values('H1','1',to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat_index values('H2','1',to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat_index values('H4','1',to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat_index values('H4A','1',to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat_index values('H4','1',to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat_index values('H4A','1',to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat_index values('H6A','1',to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,'H6');
insert into pre_seat_index values('H6B','1',to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,'H6');

/*
drop table pre_seat;
create table pre_seat (	--/* 母表 
	Pre_ID varchar2(17), -- 索引，由pre_seat_index里的字段拼凑而来：
-- Train_no:itam:beg_date(YYYYMMDD)
	Carno number(2), -- 车厢号，不分车厢的票车厢号＝0 
	seat_type number(4),  --席别，0＝无号 
	beg_seatno number(3), -- 开始席号
	end_seatno number(3),-- 结束席号
	beg_station number(3), -- 上车站 
	end_station number(3),  -- 最远站 
	shortest_station number(3),  -- 限售以远 
	purpose	number(9),		-- 用途 
	gride   varchar2(50),   -- 列车等级，新空调直达特快等。
	primary key (Pre_ID,Carno,beg_seatno,beg_station)
)
*/
insert into pre_seat values('H1:1:20080201',1,12,1,120,0,5,0,0,'G04');
insert into pre_seat values('H1:1:20080201',2,12,1,120,0,5,0,0,'G04');
insert into pre_seat values('H1:1:20080201',3,12,1,120,0,1,0,0,'G04');
insert into pre_seat values('H1:1:20080201',3,12,1,120,1,5,0,0,'G04');
insert into pre_seat values('H1:1:20080201',4,12,1,120,0,2,0,0,'G04');
insert into pre_seat values('H1:1:20080201',4,12,1,120,2,5,0,0,'G04');
insert into pre_seat values('H1:1:20080201',5,13,1,50,0,5,0,0,'G04');
insert into pre_seat values('H1:1:20080201',5,13,51,70,0,1,0,0,'G04');
insert into pre_seat values('H1:1:20080201',5,13,51,70,1,5,0,0,'G04');
insert into pre_seat values('H1:1:20080201',5,13,71,100,0,2,0,0,'G04');
insert into pre_seat values('H1:1:20080201',5,13,71,100,2,5,0,0,'G04');
insert into pre_seat values('H2:1:20080201',1,12,1,120,0,5,0,0,'G04');
insert into pre_seat values('H2:1:20080201',2,12,1,120,0,5,0,0,'G04');
insert into pre_seat values('H2:1:20080201',3,12,1,120,0,1,0,0,'G04');
insert into pre_seat values('H2:1:20080201',3,12,1,120,1,5,0,0,'G04');
insert into pre_seat values('H2:1:20080201',4,12,1,120,0,2,0,0,'G04');
insert into pre_seat values('H2:1:20080201',4,12,1,120,2,5,0,0,'G04');
insert into pre_seat values('H2:1:20080201',5,13,1,50,0,5,0,0,'G04');
insert into pre_seat values('H2:1:20080201',5,13,51,70,0,1,0,0,'G04');
insert into pre_seat values('H2:1:20080201',5,13,51,70,1,5,0,0,'G04');
insert into pre_seat values('H2:1:20080201',5,13,71,100,0,2,0,0,'G04');
insert into pre_seat values('H2:1:20080201',5,13,71,100,2,5,0,0,'G04');
insert into pre_seat values('H4:1:20080201',1,12,1,120,0,6,0,0,'G04');
insert into pre_seat values('H4:1:20080201',2,12,1,120,0,2,0,0,'G04');
insert into pre_seat values('H4:1:20080201',2,12,1,120,2,6,0,0,'G04');
insert into pre_seat values('H4:1:20080201',3,12,1,120,0,3,0,0,'G04');
insert into pre_seat values('H4:1:20080201',3,12,1,120,3,6,0,0,'G04');
insert into pre_seat values('H4:1:20080201',4,12,1,120,0,1,0,0,'G04');
insert into pre_seat values('H4:1:20080201',4,12,1,120,1,6,0,0,'G04');
insert into pre_seat values('H4:1:20080201',5,20,1,20,0,6,0,0,'G04');
insert into pre_seat values('H4:1:20080201',5,20,21,22,0,1,0,0,'G04');
insert into pre_seat values('H4:1:20080201',5,20,21,22,1,6,0,0,'G04');
insert into pre_seat values('H4:1:20080201',6,13,1,20,0,6,0,0,'G04');
insert into pre_seat values('H4:1:20080201',6,13,21,30,0,1,0,0,'G04');
insert into pre_seat values('H4:1:20080201',6,13,21,30,1,6,0,0,'G04');
insert into pre_seat values('H4:1:20080201',6,13,31,60,0,2,0,0,'G04');
insert into pre_seat values('H4:1:20080201',6,13,31,60,2,6,0,0,'G04');
insert into pre_seat values('H4:1:20080201',6,13,61,80,0,3,0,0,'G04');
insert into pre_seat values('H4:1:20080201',6,13,61,80,3,6,0,0,'G04');
insert into pre_seat values('H4:1:20080201',6,13,81,100,0,6,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',1,12,1,120,0,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',2,12,1,120,0,2,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',2,12,1,120,2,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',3,12,1,120,0,3,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',3,12,1,120,3,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',4,12,1,120,0,1,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',4,12,1,120,1,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',5,20,1,20,0,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',5,20,21,22,0,1,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',5,20,21,22,1,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',6,13,1,20,0,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',6,13,21,30,0,1,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',6,13,21,30,1,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',6,13,31,60,0,2,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',6,13,31,60,2,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',6,13,61,80,0,3,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',6,13,81,100,0,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080201',6,13,61,80,3,5,0,0,'G04');

insert into pre_seat values('H6A:1:20080401',1,12,1,120,0,5,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',2,12,1,120,0,1,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',2,12,1,120,1,5,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',3,12,1,120,0,2,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',3,12,1,120,2,5,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',4,12,1,120,0,5,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',5,20,1,20,0,5,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',5,20,21,22,0,2,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',5,20,21,22,2,5,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',6,13,1,50,0,5,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',6,13,51,60,0,1,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',6,13,51,60,1,5,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',6,13,61,80,0,2,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',6,13,61,80,2,5,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',6,13,81,100,0,3,0,0,'G04');
insert into pre_seat values('H6A:1:20080401',6,13,81,100,3,5,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',1,12,1,120,0,6,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',2,12,1,120,0,2,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',2,12,1,120,2,6,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',3,12,1,120,0,3,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',3,12,1,120,3,6,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',4,12,1,120,0,1,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',4,12,1,120,1,6,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',5,20,1,10,0,6,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',5,20,11,20,0,1,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',5,20,11,20,1,6,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',5,20,21,22,0,3,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',5,20,21,22,3,6,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',6,13,1,20,0,6,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',6,13,21,50,0,1,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',6,13,21,50,1,6,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',6,13,51,60,0,2,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',6,13,51,60,2,6,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',6,13,61,80,0,3,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',6,13,61,80,3,6,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',6,13,81,100,0,4,0,0,'G04');
insert into pre_seat values('H6B:1:20080401',6,13,81,100,4,6,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',1,12,1,120,0,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',2,12,1,120,0,2,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',2,12,1,120,2,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',3,12,1,120,0,3,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',3,12,1,120,3,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',4,12,1,120,0,1,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',4,12,1,120,1,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',5,20,1,20,0,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',5,20,21,22,0,1,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',5,20,21,22,1,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',6,13,1,20,0,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',6,13,21,30,0,1,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',6,13,21,30,1,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',6,13,31,60,0,2,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',6,13,31,60,2,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',6,13,61,80,0,3,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',6,13,61,80,3,5,0,0,'G04');
insert into pre_seat values('H4A:1:20080401',6,13,81,100,0,5,0,0,'G04');

insert into pre_seat values('H4:1:20080401',1,12,1,120,0,6,0,0,'G04');
insert into pre_seat values('H4:1:20080401',2,12,1,120,0,2,0,0,'G04');
insert into pre_seat values('H4:1:20080401',2,12,1,120,2,6,0,0,'G04');
insert into pre_seat values('H4:1:20080401',3,12,1,120,0,3,0,0,'G04');
insert into pre_seat values('H4:1:20080401',3,12,1,120,3,6,0,0,'G04');
insert into pre_seat values('H4:1:20080401',4,12,1,120,0,1,0,0,'G04');
insert into pre_seat values('H4:1:20080401',4,12,1,120,1,6,0,0,'G04');
insert into pre_seat values('H4:1:20080401',5,20,1,20,0,6,0,0,'G04');
insert into pre_seat values('H4:1:20080401',5,20,21,22,0,1,0,0,'G04');
insert into pre_seat values('H4:1:20080401',5,20,21,22,1,6,0,0,'G04');
insert into pre_seat values('H4:1:20080401',6,13,1,20,0,6,0,0,'G04');
insert into pre_seat values('H4:1:20080401',6,13,21,30,0,1,0,0,'G04');
insert into pre_seat values('H4:1:20080401',6,13,21,30,1,6,0,0,'G04');
insert into pre_seat values('H4:1:20080401',6,13,31,60,0,2,0,0,'G04');
insert into pre_seat values('H4:1:20080401',6,13,31,60,2,6,0,0,'G04');
insert into pre_seat values('H4:1:20080401',6,13,61,80,0,3,0,0,'G04');
insert into pre_seat values('H4:1:20080401',6,13,81,100,0,6,0,0,'G04');
insert into pre_seat values('H4:1:20080401',6,13,61,80,3,6,0,0,'G04');

--create table seat (		/* 席位表 */
--	start_date	date,	/* 始发日期 YYYY-MM-DD*/
--	beg_station number(3),	/* 上车站 */
--	Train_no varchar2(20),	/* 始发车次 */
--	Train_Carno number(2), /* 车厢号，不分车厢的票车厢号＝0 */
--	seat_type number(4),	/*席别，0＝无号*/
--	seat_no   number(3),	/* 席位号 */
--	end_station number(3),	/* 售前是最远站，售后改为下车站 */
--	shortest_station number(3),	/* 限售以远 */
--	purpose	number(9),		/* 用途 */
--	flag	number(1),	/* 标志：0正常,1占用,2已售,-1禁售 */
--	used_dev varchar2(16), /* 最后操作终端 */
--	used_uid varchar2(16), /* 最后操作人 */
--	used_time date ,		/* 最后操作时间 YYYY-MM-DD HH24:MI:SS */
--	primary key(start_date,beg_station,Train_no,Train_carno,
--			seat_type,seat_no)
--);

-- create table seat_remant ( 	/* 存量表 */
-- 	start_date	date,	/* 始发日期 */
-- 	beg_station number(3),	/* 上车站 */
-- 	Train_no varchar2(20),		/* 始发车次 */
-- 	Train_Carno number(2), /* 车厢号，不分车厢的票车厢号＝0 */
-- 	seat_type number(4),	/*席别，0＝无号*/
-- 	purpose	number(9),		/* 用途 */
-- 	end_station number(3),	/* 最远站 */
-- 	amoung	number(3),
-- 	primary key(start_date,beg_station,Train_no,Train_carno,
-- 		seat_type,purpose,end_station)
-- ) ;
	
