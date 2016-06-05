/* 看表空间利用率 */  

SELECT DBName.name, sysdate, Total.name "Tablespace Name", 
       num_db_files, Free_space, ((100 * Free_space) / total_space) "%Free", 
       (total_space - Free_space) Used_space, 
       ((100 * (total_space - free_space)) / total_space) "%Used", total_space 
FROM (select tablespace_name, sum(bytes / 1024) Free_Space from sys.dba_free_space group by tablespace_name) Free, 
     (select b.name, count(*) num_db_files, sum(bytes / 1024) TOTAL_SPACE from sys.v_$datafile a, sys.v_$tablespace B 
       where a.ts# = b.ts# group by b.name) Total, (select name from sys.v_$database) DBName 
WHERE Free.Tablespace_name = Total.name and DBName.name = DBName.name; 

SELECT c.tablespace_name tablespace_name, ROUND(a.bytes/1048576,2) total_size,ROUND((a.bytes-b.bytes)/1048576,2) used_size,
	   ROUND(b.bytes/1048576,2) free_size, 
	   ROUND(b.bytes/a.bytes * 100,2)||'%' free_pct FROM 
	   (SELECT tablespace_name,SUM(a.bytes) bytes 
		FROM sys.DBA_DATA_FILES a 
		GROUP BY tablespace_name) a, 
	   (SELECT a.tablespace_name, 
		NVL(SUM(b.bytes),0) bytes 
		FROM sys.DBA_DATA_FILES a, 
		sys.DBA_FREE_SPACE b 
		WHERE a.tablespace_name = b.tablespace_name (+) 
		AND a.file_id = b.file_id (+) 
		GROUP BY a.tablespace_name) b, 
	   sys.DBA_TABLESPACES c 
	   WHERE a.tablespace_name = b.tablespace_name(+) 
		AND a.tablespace_name = c.tablespace_name 
		ORDER BY ROUND(b.bytes/1048576,2);


