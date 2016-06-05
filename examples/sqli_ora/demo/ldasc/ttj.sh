./trun.sh
time ./t_OAD -f $HOME/config/ld.ini  tjrb <../sql/TJ.txt
cms_acc <<!
select count(1) from tjrb;
exit
!
