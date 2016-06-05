SELECT	
	to_char(a.on_date,'YYYY-MM-DD') on_date,
	a.run_train,
	a.carno,
	decode(a.seat_type, 21,20, 22,20, 23,20, 31,30, 32,30, seat_type) seat_type,
	a.beg_station beg_station,
	b.station_code end_station,
	a.purpose
	,count(*) quantity
FROM seat a,stop_station b
WHERE 	a.FLAG=0 
	AND to_char(on_date,'YYYY-MM-DD') = '2009-05-06'
	AND beg_station = 'BEP'
	AND purpose=0 
	AND b.train_no = a.train_no
	AND b.sequence = a.end_station
	AND b.end_date >= to_date('2009-05-06','YYYY-MM-DD')
group by a.on_date,a.run_train,a.carno,decode(a.seat_type, 21,20, 22,20, 23,20, 31,30, 32,30, seat_type),a.beg_station,b.station_code,a.purpose;
