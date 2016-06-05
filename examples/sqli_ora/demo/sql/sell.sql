/* default with not null */

drop table sell;
create table sell (
	devid		varchar2(16),	/* 窗口号 */
	ticket_no	char(8),	/* 票号 */
	stat_date	date,		/* 统计日期  */
	shift_code	number(2),	/* 班次代码＝(夜班＝0-4，白班＝5-9) */
	shift_flg 	char(2),              -- 班别　ABCDR，BY
	sell_time	date,		/* 售票时间 YYYY-MM-DD HH24:MI:SS*/
	stat		number(1),	/* 状态：0正常，-1作废，1退票,2检票 */
	userid		varchar2(16),	/* 操作员ID */
	username	varchar2(32),	/* 操作员名 */
	on_time		date,		/* 上车时间 YYYY-MM-DD HH24:MI */
	off_time	date,		/* 下车时间 YYYY-MM-DD HH24:MI */
	valid_days   number(2),	/* 有效天数 */
	start_date	date,		/* 始发日期，退票用 */
	train_no	varchar2(12),	/* 全车次 */
	run_train	varchar2(6),	/* 运行车次 */
	carno		number(2),	/* 车厢号 */
	seat_no		number(3),	/* 席位号 */
	seat_type	number(3),	/* 席别 */
	beg_station		varchar(4),	/* 上车站 */
	station_name	varchar2(12),	/* 上车站名 */
	station_off	varchar2(4),	/* 下车站 */
	station_off_name	varchar2(12),	/* 下车站名 */
	station_end	varchar2(4) null,	/* 终到站,通票 */
	station_end_name	varchar2(12) null,	/* 终到站名,通票 */
	p_through	varchar2(40) null,	/* 经由代码 */
	distance	number(6),	/* 本车次里程 */
	total_distance  number(6),	/* 总里程 */
	gride		varchar2(50),	/* 事由代码 */
	ticket_type	number(2),	/* 票种代码 0全,1孩,2学,3残,4免,5探,6赠,7空,8补,9签*/
	money		number(12,2),	/* 票价 */
	addtions_money1   number(8,2),	/* 附加费1 软票费*/
	addtions_money2   number(8,2),	/* 附加费2 车站空调费*/
	addtions_money3   number(8,2),	/* 附加费3 卧铺订票费*/
	addtions_money4   number(8,2),	/* 附加费4 氧气费*/
	addtions_money5   number(8,2),	/* 附加费5 */
	discount	number(8,3),	/* 折扣费 */
	total_fee	number(12,2),	/* 总价 */
	total_money	number(12,2),	/* 其中现金 */
	pay_flag	number(1),	/* 付款标志： 
			0:现金，1刷卡 */
	pay_flag2	number(1),	/* 付款时机： 
			0:现金，2先付，3，后付     */
	pay_id		varchar2(60) null,	/* 付款ID */
	tride_id	varchar2(20) null,	/* 交易ID */
	barcode		varchar2(256) null,	/* 条码  */
	chksum		number(12),	/* 校验和 */
	syncflg		number(1),	/* 同步标志，0已同步，1未同步 */
	note		varchar2(64), /* 备注，限乘当日当次等 */
	primary key(devid,ticket_no,stat_date)
);
-- create index account_idx on sell(devid,stat_date,shift_code);

create table tride_pnr (
	tride_id	varchar2(20) ,
	devid		varchar2(16),
	pnrid		varchar2(16) null,
	tride_time	date,
	ticket_num	number(4),
	money		number(12,2),
	primary key(tride_id,devid)
);
create	index pnr_idx on tride_pnr (pnrid,tride_time desc);

create table pnr (
	pnrid		varchar2(16) primary key,
	pnrname		varchar2(16),
	pnrtype		number(1),		/* 合同户，个人户 */
	telphone	varchar2(16) null,
	mobile		varchar2(16) null,
	addr		varchar(40) null,
	zipcode		varchar(16) null,
	email		varchar(40) null,
	principl	varchar2(16) null,	/* 负责人 */
	pass_type	varchar(10) null, 	/* 证件类型 */
	pass_no		varchar(20) null,	/* 证件号 */
	pnr_level	number(2),			/* 等级 */
	award		number(9),			/* 积分 */
	build_date	date,
	last_time	date null
);


/**************************************************************
 * -- 设备班次表
 * 每个窗口只有一条记录。初始可以没有记录，第一次登录时产生。
 **************************************************************/
--drop table dev_shift;
create table dev_shift (  
	devid varchar2(16) primary key,
	stat_date date null,		-- 统计日期
	shift_flg char(2),		-- 班别　ABCDR
	shift_code number(2) null,	-- 0-4夜班，5-9白班。
	userid varchar2(16) null,	-- 作业人ID,空为未登录
	log_time date null,		-- 登录时间
	beg_no char(8) null		-- 票据起号
);
/**************************************************************
 * -- 设备历史班次表
 * 交班时由dev_shift产生，结账时使用。
 * 存根表的票号范围
 **************************************************************/
--drop table dev_shift_his;
create table dev_shift_his (  
	devid varchar2(16) ,
	stat_date date ,		-- 统计日期
	shift_flg char(2),		-- 班别　ABCDR
	shift_code number(2) ,	-- 0-4白班，5-9夜班。
	userid varchar2(16) ,	-- 作业人ID
	username varchar2(16) ,	-- 作业人名
	log_time date ,		-- 登录时间
	logout_time date ,		-- 登出时间
	beg_no char(8),		-- 票据起号
	end_no char(8),		-- 票据止号
	bill_num number(5),	-- 票据使用量
	primary key (devid,stat_date,shift_code)
);


/*************************************************************
 * 窗口票据表--票卷管理使用
 *************************************************************/
create table dev_bill (
	devid varchar2(16) primary key,
	bill_num char(8) null,	--当前票号
	vol0 char(8) null,	-- 卷号
	vol1 char(8) null,	-- 卷号
	vol2 char(8) null,	-- 卷号
	vol3 char(8) null,	-- 卷号
	vol4 char(8) null,	-- 卷号
	vol5 char(8) null,	-- 卷号
	vol6 char(8) null,	-- 卷号
	vol7 char(8) null,	-- 卷号
	vol8 char(8) null,	-- 卷号
	vol9 char(8) null,	-- 卷号
	vol10 char(8) null,	-- 卷号
	vol11 char(8) null,	-- 卷号
	vol12 char(8) null,	-- 卷号
	volp number(2) default 0
);
