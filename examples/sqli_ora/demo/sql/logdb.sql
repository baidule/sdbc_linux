/* tuxedo 设备认证表 
   所有设备必须在这里备案 */
create table tuxdev (
	devid varchar2(16) primary key,  --  窗口号
	devname varchar2(32) null, --设备名称
	devtype number(4), -- 1:售票终端,2结账终端，4调度终端。
	devca	varchar2(53) null, -- 设备指纹 还应该有公开密钥?
	unit     number(9),  -- 所属单位代码
	atime	number(3),-- 白班开班时间　0-719分钟数,0代表单班,如AFC、代售点等
	ptime number(4), --　夜班开班时间　720-1440分钟数
	perm   varchar2(32),	-- 权限
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

/**********************************************************
 * tuxedo服务器的客户上下文文件，客户登录时建立一个记录
 * 进入服务时根据clientID取出经过认证的权限
 * logout时删除该记录
 *********************************************************/

drop table tuxcontex;
create table tuxcontex (
	clid   number(18) primary key,   -- tuxedo clientID
	devid varchar2(16),     -- 窗口号
	userid varchar2(16),    -- 用户ID
	username varchar2(32),    -- 用户名
	stat_date date,		-- 统计日期
	shift_code number(2),	-- 班次代码，-1:没有班次
	devtype number(4),	-- 窗口类型，1=售票窗口
	unit     number(9),  -- 所属单位代码
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
create index del_idx on tuxcontex(last_oper);

create table tuxuser (
	userid varchar2(16) primary key,
	username varchar(32) null,
	password varchar(16) null,
	unit     number(9),  -- 所属单位代码
	last_log date null, -- 最后登录时间
	flag	number(1)  -- 0:有效
);
