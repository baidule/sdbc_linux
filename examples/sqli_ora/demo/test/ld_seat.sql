insert into PARTTERN_COL values('SEAT','START_DATE','CH_DATE',11,'YYYY-MM-DD');
insert into PARTTERN_COL values('SELL','START_DATE','CH_JUL',4,'YYYY-MM-DD');
insert into PARTTERN_COL values('SELL','STAT_DATE','CH_JUL',4,'YYYY-MM-DD');
insert into PARTTERN_COL values('SELL','ON_TIME','CH_DATE',17,'YYYY-MM-DD HH24:MI');
insert into PARTTERN_COL values('SELL','OFF_TIME','CH_DATE',17,'YYYY-MM-DD HH24:MI');
insert into PARTTERN_COL values('SELL','CHKSUM','CH_INT4',4,null);

insert into PARTTERN_COL values('DEV_SHIFT','STAT_DATE','CH_JUL',4,'YYYY-MM-DD');
insert into PARTTERN_COL values('DEV_SHIFT','LOG_TIME','CH_TIME',8,null);
insert into PARTTERN_COL values('DEV_SHIFT_HIS','STAT_DATE','CH_JUL',4,'YYYY-MM-DD');
insert into PARTTERN_COL values('DEV_SHIFT_HIS','LOG_TIME','CH_TIME',8,null);
insert into PARTTERN_COL values('DEV_SHIFT_HIS','LOGOUT_TIME','CH_TIME',8,null);

commit;
