create table cron (
	id number(4) primary key,
	time_exp varchar2(256), -- 时间表达式,如果是触发子：起始时间，间隔时间（秒），次数（空为永远循环）
	command  varchar2(256), -- 执行的任务
	note     varchar2(256), -- 注释
	task_type number(1)	-- 任务类型，0:CRON，1:SimpleTrigger,2:JAVA类,3:SQL语句,+10:触发子
);
