/*
select  TABLE_NAME ,INDEX_NAME,INDEX_TYPE,UNIQUENESS,NUM_ROWS,DEGREE
from dba_indexes
where TABLE_NAME='SEAT';
select * from user_ind_columns  where index_name='Ë÷ÒýÃû'

*/
select  COLUMN_NAME,COLUMN_POSITION,COLUMN_LENGTH,CHAR_LENGTH,DESCEND 
from DBA_IND_COLUMNS
where table_name='SEAT' and INDEX_NAME='SYS_C006084';
