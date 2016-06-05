update stop_station set area_flg=1 where (select area from station where station_code = stop_station.station_code) is not null;
commit;
