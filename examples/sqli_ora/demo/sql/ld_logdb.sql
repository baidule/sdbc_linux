/* tuxedo 设备认证表 
   所有设备必须在这里备案 
drop table tuxdev;
create table tuxdev (
	devid varchar2(16) primary key,  -- 
	devname varchar2(32) null, --设备名称
	devtype number(4), -- 1:售票终端。
	devca	varchar2(53) null, -- 设备指纹
	unit     number(12),  -- 所属单位代码
	atime     number(3),  -- 
	ptime     number(4),  -- 
	perm   varchar2(32),    -- 权限
        purpose1 number(12),    -- 可使用的用途1
        purpose2 number(12),    -- 可使用的用途2
        purpose3 number(12),    -- 可使用的用途3
        purpose4 number(12),    -- 可使用的用途4
        purpose5 number(12),    -- 可使用的用途5
        purpose6 number(12),    -- 可使用的用途6
        purpose7 number(12),    -- 可使用的用途7
        purpose8 number(12),    -- 可使用的用途8
	clid   number(18) null,   -- tuxedo clientID
	last_log date null, -- 最后登录时间 YYYY-MM-DD HH24:MI:SS
	flag	number(1)  -- 0:有效
);
insert into tuxdev values('SPTEST','黄伟伟的LINUX',3,null,1001,480,1140,'',0,0,0,0,0,0,0,0,0,null,0);
insert into tuxdev values('SP0102003','黄鹏的虚拟机',3,null,1001,480,1140,'',0,0,0,0,0,0,0,0,0,null,0);
insert into tuxdev values('WEB_JAVA','售票测试WEB服务',1,null,1001,480,1140,'',0,0,0,0,0,0,0,0,0,null,0);
insert into tuxdev values('AFC1','自动售票机1',3,null,1001,0,1440,'',0,0,0,0,0,0,0,0,0,null,0);
insert into tuxdev values('SP0102002','售票测试终端2',3,null,1001,480,1140,'',0,0,0,0,0,0,0,0,0,null,0);
insert into tuxdev values('JOLTTEST','JOLT测试终端',3,null,1001,480,1140,'',0,0,0,0,0,0,0,0,0,null,0);
insert into tuxdev values('JOLTTEST1','黄伟伟的JOLT测试终端',3,null,1001,480,1140,'',0,0,0,0,0,0,0,0,0,null,0);
*/
/**********************************************************
 * tuxedo服务器的客户上下文文件，客户登录时建立一个记录
 * 进入服务时根据clientID取出经过认证的权限
 * logout时删除该记录
drop table tuxcontex;
create table tuxcontex (
	clid   number(18) primary key,   -- tuxedo clientID
	devid varchar2(16),     -- 窗口号
	userid varchar2(16),    -- 用户名
	username varchar(32) null,
	stat_date date,         -- 统计日期
        shift_code number(1),   -- 班次代码，-1:没有班次
	devtype number(4), -- 1:售票终端。
	unit     number(12),  -- 所属单位代码
	perm   varchar2(32),	-- 权限
	purpose1 number(12),    -- 可使用的用途1
	purpose2 number(12),    -- 可使用的用途2
	purpose3 number(12),    -- 可使用的用途3
	purpose4 number(12),    -- 可使用的用途4
	purpose5 number(12),    -- 可使用的用途5
	purpose6 number(12),    -- 可使用的用途6
	purpose7 number(12),    -- 可使用的用途7
	purpose8 number(12),    -- 可使用的用途8
	last_oper date null    -- 最后操作时间YYYY-MM-DDHH24:MI:SS
);
 *********************************************************/

/*
create table tuxuser (
	userid varchar2(32) primary key,
	username varchar(32) null,
	password varchar(16) null,
	unit     number(12),  -- 所属单位代码
	last_log date null, -- 最后登录时间
	flag	number(1)  -- 0:有效
);
*/
insert into tuxuser values('hp','黄鹏',null,1001,null,0);
insert into tuxuser values('hww','黄维伟',null,1001,null,0);
insert into tuxuser values('ylh','余立华',null,1001,null,0);
insert into tuxuser values('llw','李良文',null,1001,null,0);
insert into tuxuser values('WEBLOGIC','测试网站',null,1000,null,0);
insert into tuxuser values('AFCBJN','北京南站自动售票员',null,1000,null,0);
