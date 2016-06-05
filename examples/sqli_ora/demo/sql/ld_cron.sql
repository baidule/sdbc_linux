insert into cron values(1,'0 0 * * * *','/home/sdbc/bin/tcron.sh','TEST Only',0);
insert into cron values(3,'1 0 * * * *','/home/sdbc/bin/clean.sh','清理过期数据',0);
insert into cron values(2,'0 2 * * * *','/home/sdbc/bin/xwfb.sh','席位发布',0);
