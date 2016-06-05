delete tj18d where tjdate=to_date('2007-06-01','YYYY-MM-DD') and unit='BJ' and tabname='XB1';
insert into tj18d values(to_date('2007-06-01','YYYY-MM-DD'),'BJ','XB1',101,101,102,103,104,105,106,107,108,109,1010,1011,1012,1013,1014,1015,1016,1017,1018,1019,1020,1021,1022,1023,1024,1025,1026,1027,1028,1029);
commit;
