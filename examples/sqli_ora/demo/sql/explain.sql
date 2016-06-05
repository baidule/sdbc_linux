delete plan_table;
commit;
set autotrace trace exp;
select /*+role*/ Train_no,to_char(beg_date,'YYYY-MM-DD') beg_date,to_char(end_date,'YYYY-MM-DD') end_date,start_station,end_station,station_num,total_distance,start_time,iterms,cycle,gride,price_mode,discount_mode,days,flag,owner,note from ticket.TRAIN where Train_no='H4A' and end_date>=to_date('2008-01-17','YYYY-MM-DD') and flag=0;

select /*+INDEX(seat  SYS_C006084)*/ * from seat 
where start_date=to_date('2008-08-19','YYYY-MM-DD') and beg_station='BNHP' and flag=0 and Train_no='H6A';
