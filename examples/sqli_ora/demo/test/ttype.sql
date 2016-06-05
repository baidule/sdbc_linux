drop table ttype;
create table ttype (
	"id" number(2),
	"i64" number(20),
	"i2" number(4),
	"d8" number(12,2),
	"d4" number(4,2),
	"t1" timestamp default systimestamp,
	"r5" raw(128),
	"date1" date default sysdate,
	primary key ("t1")
);
