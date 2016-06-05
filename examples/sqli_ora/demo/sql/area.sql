-- set timi on;
set pagesize 0 feedback off verify off heading off echo off

select station_code code,station_name name,area from station where area is not null order by area;
