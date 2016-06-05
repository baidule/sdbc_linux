 select table_name,index_name,uniqueness,include_column,num_rows from user_indexes where table_name= 'SEAT';
-- select * from all_indexes where table_name= 'DIC_UPDATE_TABLE_INFO';
select table_name,column_name,COLUMN_POSITION,DESCEND  from all_ind_columns where index_name='SEAT_RELEASE';

select a.table_name,a.index_name,b.column_name,b.COLUMN_POSITION from user_indexes a,all_ind_columns b 
where a.table_name='SEAT' and a.uniqueness='UNIQUE' and a.table_name=b.table_name 
order by a.table_name,a.index_name,b.COLUMN_POSITION;



