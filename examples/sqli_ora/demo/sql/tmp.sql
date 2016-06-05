
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
	
