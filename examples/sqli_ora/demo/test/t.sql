declare
ro varchar2(19);
begin

select rowid from seat where start_date=to_date('2009-05-18','YYYY-MM-DD') AND beg_station='BEP' AND train_no='H6B'
AND carno=5 AND seat_type=21 AND seat_no= 11;
update seat set gride='G04 ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890' where ROWID='AAAM3iAAFAAAKBCAA6' returning ROWID into ro;
commit;
select rowid,gride  from seat where start_date=to_date('2009-05-18','YYYY-MM-DD') AND beg_station='BEP' AND train_no='H6B'
AND carno=5 AND seat_type=21 AND seat_no= 11;

end
/
