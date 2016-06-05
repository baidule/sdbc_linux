/* 本文件是临时文件，正本在sell.sql里　*/
/**************************************************************
 * -- 设备班次表
 * 每个窗口只有一条记录。初始可以没有记录，第一次登录时产生。
 **************************************************************/
create table dev_shift (  
	devid varchar2(16) primary key,
	stat_date date null,		-- 统计日期,空为未登录
	shift_code number(1) null,	-- 0-4白班，5-9夜班
	userid varchar2(16) null,	-- 作业人ID
	log_time date null,		-- 登录时间
	beg_no char(8) null		-- 票据起号
);
/**************************************************************
 * -- 设备历史班次表
 * 交班时由dev_shift产生，结账时使用。
 * 存根表的票号范围
 **************************************************************/
create table dev_shift_his (  
	devid varchar2(16) ,
	stat_date date ,		-- 统计日期
	shift_code number(1) ,	-- 0-4白班，5-9夜班。
	userid varchar2(16) ,	-- 作业人ID
	username varchar2(16) ,	-- 作业人名
	log_time date ,		-- 登录时间
	logout_time date ,		-- 登出时间
	beg_no char(8),		-- 票据起号
	end_no char(8),		-- 票据止号
	primary key (devid,stat_date,shift_code)
);


/*************************************************************
 * 窗口票据表--票卷管理使用
 *************************************************************/
drop table dev_bill;
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
