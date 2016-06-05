ret = OCI_Prepare(st,"UPDATE pt10_seat_centre"
" SET assign_station = :board_station,"
	" station_train_code = :station_train_code,"
   	"	travel_date = :travel_date,"
       	" assign_station_seq = :board_station_seq,"
       	" limit_station_seq = :arrival_station_seq,"
       	" far_from_station_seq = :board_station_seq,"
       	" seat_reuse_flag=:seat_reuse_flag"
 	" WHERE start_train_date = :start_train_date"
  " AND train_code = :train_code"
  " AND board_station = :board_station"
	  " AND coach_no = :coach_no"
	  " AND seat_no = :seat_no"
	  " AND seat_type = :seat_type"
	  " AND coach_level = :coach_level"
	  " AND bed_level = :bed_level");



---------------------

/*************************************************
Function: update_after_reuse_dao
Desc		:	生成复用席位时修改原席位
Author	: 于顺江
Version	: 1.0
Date		:	2010-5-14 10:38:53	 
Input		: after_reuse_map:{board_station,board_station_seq,arrival_station_seq,start_train_date,train_code,
													coach_no,seat_no,seat_type,coach_level,bed_level}   
Output	: stmt：由错误发生时，返回错误异常信息，
Return	: 成功返回0，失败返回-1
*************************************************/
int update_after_reuse_dao(map_t after_reuse_map,char *stmt)
{
	int ret;
	OCI_Statement* st = NULL;    //OCI_Statement对象

	 /*利用全局数据库连接句柄，生成OCI_Statement对象*/
	st  = OCI_StatementCreate(tbs_connect.dbh);
	if(NULL == st){
		tbs_get_error(stmt); 
		tbs_show_log(TBS_LOG_ERROR, "ST_ERROR:%s:%s:%d:stmt:%s",__FILE__, __FUNCTION__, __LINE__, stmt);
		return -1;
	}

	ret = OCI_Prepare(st,"UPDATE pt10_seat_centre"
   										" SET assign_station = :board_station,"
   										" station_train_code = :station_train_code,"
   										"	travel_date = :travel_date,"
							       	" assign_station_seq = :board_station_seq,"
							       	" limit_station_seq = :arrival_station_seq,"
							       	" far_from_station_seq = :board_station_seq,"
							       	" seat_reuse_flag=:seat_reuse_flag"
										 	" WHERE start_train_date = :start_train_date"
										  " AND train_code = :train_code"
										  " AND board_station = :board_station"
										  " AND coach_no = :coach_no"
										  " AND seat_no = :seat_no"
										  " AND seat_type = :seat_type"
										  " AND coach_level = :coach_level"
										  " AND bed_level = :bed_level");
	if(FALSE == ret){
		tbs_get_error(stmt); 
		tbs_show_log(TBS_LOG_ERROR, "ST_ERROR:%s:%s:%d:stmt:%s",__FILE__, __FUNCTION__, __LINE__, stmt);
		OCI_StatementFree(st);
		return -1;
	} 
	
	/*绑定变量*/
	ret = bind_by_map(st,after_reuse_map,stmt);		
	if(-1 == ret){
		tbs_get_error(stmt);
		tbs_show_log(TBS_LOG_ERROR, "ST_ERROR:%s:%s:%d:stmt:%s",__FILE__, __FUNCTION__, __LINE__, stmt);
		OCI_StatementFree(st);
		return -1;
	}
	
	ret = OCI_Execute(st);
	if(FALSE == ret){
		tbs_get_error(stmt);
		tbs_show_log(TBS_LOG_ERROR, "ST_ERROR:%s:%s:%d:stmt:%s",__FILE__, __FUNCTION__, __LINE__, stmt);
		OCI_StatementFree(st);
		return -1;	
	}
	tbs_show_log(TBS_LOG_DEBUG, "ST:%s:%s:修改原席位成功！",__FILE__, __FUNCTION__);
	
	/*释放资源*/
	OCI_StatementFree(st);
	return 0;
}

