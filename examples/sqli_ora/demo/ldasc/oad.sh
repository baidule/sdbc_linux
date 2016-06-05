#╡БйтЁ╛╪╤╪стьфВ
./ora.sh <trun.sql
time ./oad -f ld.ini cm_info_route <cm_info_route.txt
./ora.sh <<!
select count(*) from cm_info_route;
!
