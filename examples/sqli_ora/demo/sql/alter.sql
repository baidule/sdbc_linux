/*
 set pagesize 0 feedback off verify off heading off echo off

alter table train modify (
	start_station varchar2(10),
	end_station varchar2(10)
)
;
alter table train add (
	seat_type varchar(10)
)
alter table stop_station  add (
    line  number(4) default 0
)
alter table stop_station  modify (
	stop_time number(3)
)
*/
alter table PATTERN_COL  add (
	PSEUDO_NAME varchar2(24)
)
;
