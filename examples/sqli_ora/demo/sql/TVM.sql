drop table tvmctx;
create table tvmctx (
	stn_code char(3),
	opid	 char(4),
	devid	varchar2(16),
	ip_addr	varchar2(15),
	cc_card	varchar2(9),
	userid	varchar2(16),
	bill_num	char(7),
	next_vol	char(7),
	last_time	date,
	primary key (stn_code,opid)
);
