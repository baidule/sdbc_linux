set timi on;
select station_code code,station_name name,line,station_type tp,area 
from station 
where area=(select area from station where station_code='TGP');

select a.station_code code,a.station_name name,a.line line,a.station_type tp,a.area 
from station a,station b
where a.area=b.area and b.station_code='TGP';
