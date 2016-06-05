/* default with not null */
--create table pre_seat (		/* 母表 */
--	Train_no varchar2(20),	/* 始发车次 */
--	item	char(1) default '1',		/* 组号，一般＝'1' */
--	Train_Carno number(2), /* 车厢号，不分车厢的票车厢号＝0 */
--	seat_type number(4),  /*席别，0＝无号 */
--	beg_seatno number(3),
--	end_seatno number(3),
--	amoung	number(3),
--	beg_station number(3), /* 上车站 */
--	end_station number(3),  /* 最远站 */
--	shortest_station number(3),  /* 限售以远 */
--	purpose	number(9),		/* 用途 */
--	beg_date date,	/* 有效起始日期 YYYY-MM-DD */
--	end_date date,  /* 有效结束日期 YYYY-MM-DD */
--	flag	number(1),  /* 0:有效，其它：无效*/
--	note	varchar(40),
--	distribute_time date null /* 最后发布时间 YYYY-MM-DD HH24:MI:SS*/
--);
insert into pre_seat values('H1','1',1,12,1,120,120,0,5,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H1','1',2,12,1,120,120,0,5,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H1','1',3,12,1,120,120,0,1,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H1','1',3,12,1,120,120,1,5,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H1','1',4,12,1,120,120,0,2,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H1','1',4,12,1,120,120,2,5,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H1','1',5,13,1,50,50,0,5,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H1','1',5,13,51,70,20,0,1,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H1','1',5,13,51,70,20,1,5,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H1','1',5,13,71,100,30,0,2,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H1','1',5,13,71,100,30,2,5,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H2','1',1,12,1,120,120,0,5,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H2','1',2,12,1,120,120,0,5,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H2','1',3,12,1,120,120,0,1,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H2','1',3,12,1,120,120,1,5,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H2','1',4,12,1,120,120,0,2,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H2','1',4,12,1,120,120,2,5,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H2','1',5,13,1,50,50,0,5,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H2','1',5,13,51,70,20,0,1,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H2','1',5,13,51,70,20,1,5,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H2','1',5,13,71,100,30,0,2,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H2','1',5,13,71,100,30,2,5,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',1,12,1,120,120,0,7,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',2,12,1,120,120,0,2,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',2,12,1,120,120,2,7,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',3,12,1,120,120,0,3,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',3,12,1,120,120,3,7,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',4,12,1,120,120,0,1,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',4,12,1,120,120,1,7,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',5,20,1,20,60,0,7,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',5,20,21,22,6,0,1,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',5,20,21,22,6,1,7,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,1,20,20,0,7,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,21,30,10,0,1,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,21,30,10,1,7,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,31,60,30,0,2,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,31,60,30,2,7,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,61,80,20,0,3,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,61,80,20,3,7,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,81,100,20,0,7,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',1,12,1,120,120,0,6,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',2,12,1,120,120,0,2,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',2,12,1,120,120,2,6,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',3,12,1,120,120,0,3,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',3,12,1,120,120,3,6,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',4,12,1,120,120,0,1,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',4,12,1,120,120,1,6,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',5,20,1,20,60,0,6,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',5,20,21,22,6,0,1,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',5,20,21,22,6,1,6,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,1,20,20,0,6,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,21,30,10,0,1,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,21,30,10,1,6,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,31,60,30,0,2,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,31,60,30,2,6,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,61,80,20,0,3,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,81,100,20,0,6,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,61,80,20,3,6,0,0,to_date('2008-02-01','YYYY-MM-DD'),to_date('2008-03-31','YYYY-MM-DD'),0,null,null);

insert into pre_seat values('H6A','1',1,12,1,120,120,0,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',2,12,1,120,120,0,1,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',2,12,1,120,120,1,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',3,12,1,120,120,0,2,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',3,12,1,120,120,2,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',4,12,1,120,120,0,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',5,20,1,20,60,0,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',5,20,21,22,6,0,2,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',5,20,21,22,6,2,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',6,13,1,50,50,0,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',6,13,51,60,10,0,1,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',6,13,51,60,10,1,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',6,13,61,80,20,0,2,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',6,13,61,80,20,2,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',6,13,81,100,20,0,3,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6A','1',6,13,81,100,20,3,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',1,12,1,120,120,0,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',2,12,1,120,120,0,2,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',2,12,1,120,120,2,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',3,12,1,120,120,0,3,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',3,12,1,120,120,3,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',4,12,1,120,120,0,1,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',4,12,1,120,120,1,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',5,20,1,10,30,0,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',5,20,11,20,30,0,1,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',5,20,11,20,30,1,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',5,20,21,22,6,0,3,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',5,20,21,22,6,3,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',6,13,1,20,20,0,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',6,13,21,50,30,0,1,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',6,13,21,50,30,1,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',6,13,51,60,10,0,2,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',6,13,51,60,10,2,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',6,13,61,80,20,0,3,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',6,13,61,80,20,3,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',6,13,81,100,20,0,4,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H6B','1',6,13,81,100,20,4,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,'H6',null);
insert into pre_seat values('H4','2',1,12,1,120,120,0,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',2,12,1,120,120,0,2,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',2,12,1,120,120,2,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',3,12,1,120,120,0,3,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',3,12,1,120,120,3,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',4,12,1,120,120,0,1,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',4,12,1,120,120,1,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',5,20,1,20,60,0,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',5,20,21,22,6,0,1,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',5,20,21,22,6,1,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,1,20,20,0,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,21,30,10,0,1,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,21,30,10,1,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,31,60,30,0,2,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,31,60,30,2,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,61,80,20,0,3,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,61,80,20,3,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','2',6,13,81,100,20,0,7,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',1,12,1,120,120,0,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',2,12,1,120,120,0,2,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',2,12,1,120,120,2,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',3,12,1,120,120,0,3,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',3,12,1,120,120,3,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',4,12,1,120,120,0,1,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',4,12,1,120,120,1,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',5,20,1,20,60,0,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',5,20,21,22,6,0,1,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',5,20,21,22,6,1,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,1,20,20,0,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,21,30,10,0,1,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,21,30,10,1,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,31,60,30,0,2,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,31,60,30,2,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,61,80,20,0,3,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,81,100,20,0,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);
insert into pre_seat values('H4','1',6,13,61,80,20,3,6,0,0,to_date('2008-04-01','YYYY-MM-DD'),to_date('2099-12-31','YYYY-MM-DD'),0,null,null);

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
	
