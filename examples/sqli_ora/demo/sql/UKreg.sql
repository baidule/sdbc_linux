drop table ukreg;
create  table ukreg (
	FLD_kid0 number(12),
	FLD_kid1 number(12),
	FLD_uid  varchar2(12), /* USB-Key 上的标识码 */
	FLD_userid varchar2(16), /* 数据库应用用户登录名 */ 
	FLD_devid varchar2(16),	/* 窗口号 */
	FLD_puk	varchar2(171),   /* 用户公开密钥 */
	FLD_flag	number(1),	/* 标记，0：无效，1：有效 */
	FLD_enddate date		/* 失效日期，YYYY-MM-DD,可以空 */
);
create unique index uk_pri on ukreg(FLD_kid0,FLD_kid1);
