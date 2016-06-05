
//#include <stdio.h>
#include <atmi.h>		/* TUXEDO  Header File */
#include <string.h>
#include <kpapp.h>
#include <sys/time.h>

/*查询直达车次信息*/
JSON_OBJECT get_direct_lines(char* condition)
{
	
	char *sendbuf, *rcvbuf;
	JSON_OBJECT json_ret;
	int ret;
	json_ret=json_object_new_object();
	long rcvlen;

	if((rcvbuf = (char *) tpalloc("CARRAY", NULL, 200)) == NULL) {
		(void) fprintf(stderr,"Error allocating receive buffer\n");
		return json_ret;
	}
	if((sendbuf = (char *) tpalloc("CARRAY", NULL, 2000)) == NULL) {
		(void) fprintf(stderr,"Error allocating send  buffer\n");
		tpfree(rcvbuf);
		return json_ret;
	}
 	strcpy(sendbuf,condition);
	ret = tpcall("fdzquery", (char *)sendbuf, 1+strlen(sendbuf), (char **)&rcvbuf, &rcvlen, (long)0);
	if(!ret){
		json_object_put(json_ret);
		json_ret=json_tokener_parse(rcvbuf);
	}
	tpfree(rcvbuf);
	tpfree(sendbuf);
	return json_ret;				
}

/*获得站点所在区域的所有车站信息*/
JSON_OBJECT get_area_info(char *station)
{
	JSON_OBJECT json,backjson;
	JSON_OBJECT json_ret;
	char *sendbuf, *rcvbuf;
	int ret;
	int day;
	char today[30];
	long sendlen,rcvlen;
	
	/*initialize the return value*/
	json_ret=json_object_new_array();
	day=rtoday();
	if((rcvbuf = (char *) tpalloc("CARRAY", NULL, 200)) == NULL) {
		return json_ret;
	}
	if((sendbuf = (char *) tpalloc("CARRAY", NULL, 20000)) == NULL) {
		tpfree(rcvbuf);
		return json_ret;
	}
	
	json=json_object_new_object();
	backjson=json_object_new_object();
	add_string_to_json(backjson,"station_code",station);
	rjultostrfmt(today,day+1,YEAR_TO_DAY);
	add_string_to_json(backjson,"end_date",today);
	add_string_to_json(json,"tablename","station");
	add_string_to_json(json,"where",
	 "where area=(select area from TICKET.station where station_code=:station_code  and beg_date<=:end_date and end_date >= :end_date and flag=0) and beg_date<=:end_date and end_date >= :end_date and flag=0");
	json_object_object_add(json,"values",backjson);
	strcpy(sendbuf,json_object_to_json_string(json));
	json_object_put(json);
	
	ret=tpcall("tuxselect", (char *)sendbuf, 1+strlen(sendbuf), (char **)&rcvbuf, &rcvlen, (long)0);
	if(ret){
		tpfree(sendbuf);
		tpfree(rcvbuf);
		return json_ret;
	}
	
	json_object_put(json_ret);
	json_ret=json_tokener_parse(rcvbuf);
	tpfree(sendbuf);
	tpfree(rcvbuf);
	return json_ret;	
}
JSON_OBJECT transfer_zero(char *start,char *end,char *msg)
{
	JSON_OBJECT json_start_area,json_end_area;
	JSON_OBJECT json_ret,json_temp;
	char *sendbuf, *rcvbuf;
	char condition[200];
	int ret,i,j,k;
	int start_count,end_count;
	int day;
	char today[30];
	long sendlen,rcvlen;
/*
		此函数应该先检查输入的是区域还是具体的车站代码，本示例中并没有这样做。
		本示例默认输入的是车站代码，但考虑了车站的区域性 
		
*/	
	
	/*initialize the return value*/
	day=rtoday();
	json_ret=json_object_new_object();
	json_object_object_add(json_ret,"transfer_count",json_object_new_int(-1));
	json_object_object_add(json_ret,"solutions",json_object_new_array());

	json_start_area=get_area_info(start);
	json_end_area=get_area_info(end);
	
	
	start_count=json_object_array_length(json_start_area);  //发站所在区域的车站数
	end_count=json_object_array_length(json_end_area);      //到站所在区域的车站数
	printf("start_count=%d,end_count=%d\n",start_count,end_count);
	if(start_count==0 && end_count==0){
		sprintf(condition,"%s||%s|%s",rjultostrfmt(today,day+1,YEAR_TO_DAY),start,end);
		json_object_object_add(json_ret,"transfer_count",json_object_new_int(0));
		json_object_object_add(json_ret,"solutions",get_direct_lines(condition));	
	}
	else if(start_count==0 && end_count>0){
		for(i=0;i<json_object_array_length(json_end_area);i++){
			sprintf(condition,"%s||%s|%s",rjultostrfmt(today,day+1,YEAR_TO_DAY),start,json_object_get_string(json_object_object_get(json_object_array_get_idx(json_end_area,i),"station_code")));	
			json_temp=get_direct_lines(condition);
			if(json_object_array_length(json_temp)>0){
				json_object_object_add(json_ret,"transfer_count",json_object_new_int(0));
				for(j=0;j<json_object_array_length(json_temp);j++){
					json_object_array_add(json_object_object_get(json_ret,"solutions"),json_object_array_get_idx(json_temp,j));
				}
			}
		}
	}
	else if(start_count>0 && end_count==0){
		for(i=0;i<json_object_array_length(json_start_area);i++){
			sprintf(condition,"%s||%s|%s",rjultostrfmt(today,day+1,YEAR_TO_DAY),json_object_get_string(json_object_object_get(json_object_array_get_idx(json_start_area,i),"station_code")),end);	
			json_temp=get_direct_lines(condition);
			if(json_object_array_length(json_temp)>0){
				json_object_object_add(json_ret,"transfer_count",json_object_new_int(0));
				for(j=0;j<json_object_array_length(json_temp);j++){
					json_object_array_add(json_object_object_get(json_ret,"solutions"),json_object_array_get_idx(json_temp,j));
				}
			}
		}
	}
	else{
		for(i=0;i<json_object_array_length(json_start_area);i++){
			for(j=0;j<json_object_array_length(json_end_area);j++){
				sprintf(condition,"%s||%s|%s",rjultostrfmt(today,day+1,YEAR_TO_DAY),json_object_get_string(json_object_object_get(json_object_array_get_idx(json_start_area,i),"station_code")),json_object_get_string(json_object_object_get(json_object_array_get_idx(json_end_area,j),"station_code")));	
				printf("condition =%s\n",condition);
				json_temp=get_direct_lines(condition);
//				printf("json_temp=%s\n",json_object_to_json_string(json_temp));
				if(json_object_array_length(json_temp)>0){
					json_object_object_add(json_ret,"transfer_count",json_object_new_int(0));
					for(k=0;k<json_object_array_length(json_temp);k++){
						json_object_array_add(json_object_object_get(json_ret,"solutions"),json_object_array_get_idx(json_temp,k));
//						printf("json_ret=%s\n",json_object_to_json_string(json_ret));
					}
				}
			}
		}
		
	}
	json_object_put(json_start_area);
	json_object_put(json_end_area);
	return json_ret;
	
	
}


JSON_OBJECT transfer_one(char *start,char *end,char *msg)
{
	JSON_OBJECT json_start,json_end;
	JSON_OBJECT json_start_line,json_end_line;
	JSON_OBJECT json_ret,json_solution;
    char *sendbuf, *rcvbuf;
    char condition[200];
	char code_start[20],code_end[20],run_no_start[10],run_no_end[10];
	int day;
	char today[30];
    int ret;
	long sendlen,rcvlen;
	int i,j,i_station,j_station;
	
	
	/*initialize the return value*/
	json_ret=json_object_new_object();
	json_object_object_add(json_ret,"transfer_count",json_object_new_int(-1));
	json_object_object_add(json_ret,"solutions",json_object_new_array());
	day=rtoday();
	if((rcvbuf = (char *) tpalloc("CARRAY", NULL, 200)) == NULL) {
		strcpy(msg,"Error allocating send buffer\n");
		return json_ret;
	}
	if((sendbuf = (char *) tpalloc("CARRAY", NULL, 20000)) == NULL) {
		strcpy(msg,"Error allocating receive buffer\n");
		tpfree(rcvbuf);
		return json_ret;
	}
    

	/*obtain the trains departure from start station*/
	sprintf(sendbuf, "%s||%s||",rjultostrfmt(today,day+1,YEAR_TO_DAY),start);
	ret = tpcall("fdzquery", (char *)sendbuf, 1+strlen(sendbuf), (char **)&rcvbuf, &rcvlen, (long)0);
	
	if(ret) {
		strcpy(msg,"tpcall error!");
		tpfree(rcvbuf);
		tpfree(sendbuf);
		return json_ret;
	}
	json_start=json_tokener_parse(rcvbuf);	
	
	/*obtain the trains arrive at end station*/
	sprintf(sendbuf, "%s|||%s|",rjultostrfmt(today,day+1,YEAR_TO_DAY),end);
	ret = tpcall("fdzquery", (char *)sendbuf, 1+strlen(sendbuf), (char **)&rcvbuf, &rcvlen, (long)0);
	if(ret){
		strcpy(msg,"tpcall error!");
		tpfree(rcvbuf);
		tpfree(sendbuf);
	    return json_ret;
	}
	json_end=json_tokener_parse(rcvbuf);
	
	
  	for(i=0;i<json_object_array_length(json_start);i++){
		json_start_line=json_object_array_get_idx(json_start,i);
		strcpy(run_no_start,json_object_get_string(json_object_object_get(json_object_array_get_idx(json_object_object_get(json_start_line,"stop_station"),0),"run_train_no")));
		
		for(j=0;j<json_object_array_length(json_end);j++){
			json_end_line=json_object_array_get_idx(json_end,j);
			for(i_station=0;i_station<json_object_array_length(json_object_object_get(json_start_line,"stop_station"));i_station++){
				for(j_station=0;j_station<json_object_array_length(json_object_object_get(json_end_line,"stop_station"));j_station++){
					strcpy(code_start,json_object_get_string(json_object_object_get(json_object_array_get_idx(json_object_object_get(json_start_line,"stop_station"),i_station),"station_code")));
					strcpy(code_end,json_object_get_string(json_object_object_get(json_object_array_get_idx(json_object_object_get(json_end_line,"stop_station"),j_station),"station_code")));
					if(!strcmp(code_start,code_end)) {
					    if(json_object_get_int(json_object_object_get(json_ret,"transfer_count"))!=1){
						    json_object_object_add(json_ret,"transfer_count",json_object_new_int(1));
						}
						json_solution=json_object_new_object();
						json_object_object_add(json_solution,"t1",json_object_new_string(code_start));
						sprintf(condition,"%s|%s|%s|%s",rjultostrfmt(today,day+1,YEAR_TO_DAY),run_no_start,start,code_start);
						json_object_object_add(json_solution,"s_t1",get_direct_lines(condition));
						
						/*here time may be a problem .*/
						strcpy(run_no_end,json_object_get_string(json_object_object_get(json_object_array_get_idx(json_object_object_get(json_end_line,"stop_station"),j_station),"run_train_no")));
						sprintf(condition,"%s|%s|%s|%s",rjultostrfmt(today,day+1,YEAR_TO_DAY),run_no_end,code_end,end);
						json_object_object_add(json_solution,"t1_e",get_direct_lines(condition));
						json_object_array_add(json_object_object_get(json_ret,"solutions"),json_solution);
					}
						
				}
				
			}
		}
	}	
	
	tpfree(rcvbuf);
	tpfree(sendbuf);
	json_object_put(json_start);
	json_object_put(json_end);
	return json_ret;
				 
	
}
JSON_OBJECT transfer_two(char *start,char *end,char *msg)
{
	JSON_OBJECT json_start;
	JSON_OBJECT json_start_line,json_transfer_one;
	JSON_OBJECT json_ret,json_solution;
	char *sendbuf, *rcvbuf;
	int day;
	char today[30];
	char code_start[20],run_no_start[10],run_no_end[10];
	char condition[200];
    int ret;
	long sendlen,rcvlen;
	int i,i_station;

	/*initialize the return value*/
	json_ret=json_object_new_object();
	json_object_object_add(json_ret,"transfer_count",json_object_new_int(-1));
	json_object_object_add(json_ret,"solutions",json_object_new_array());

	day=rtoday();
	if((rcvbuf = (char *) tpalloc("CARRAY", NULL, 200)) == NULL) {
		strcpy(msg,"Error allocating send buffer\n");
		return json_ret;
	}
	if((sendbuf = (char *) tpalloc("CARRAY", NULL, 20000)) == NULL) {
		strcpy(msg,"Error allocating receive buffer\n");
		tpfree(rcvbuf);
		return json_ret;
	}
	
	/*obtain the trains departure from start station*/
	sprintf(sendbuf, "%s||%s||",rjultostrfmt(today,day+1,YEAR_TO_DAY),start);
	ret = tpcall("fdzquery", (char *)sendbuf, 1+strlen(sendbuf), (char **)&rcvbuf, &rcvlen, (long)0);
	if(!ret){
		json_start=json_tokener_parse(rcvbuf);	
	}

	if(ret){
		strcpy(msg,"tpcall error!");
		tpfree(sendbuf);
		tpfree(rcvbuf);
		return json_ret;
	}
	json_start=json_tokener_parse(rcvbuf);	
	for(i=0;i<json_object_array_length(json_start);i++){
		json_start_line=json_object_array_get_idx(json_start,i);
		strcpy(run_no_start,json_object_get_string(json_object_object_get(json_object_array_get_idx(json_object_object_get(json_start_line,"stop_station"),0),"run_train_no")));
		/*从出发站的下一站开始查询，i_station=1*/
		for(i_station=1;i_station<json_object_array_length(json_object_object_get(json_start_line,"stop_station"));i_station++){
			strcpy(code_start,json_object_get_string(json_object_object_get(json_object_array_get_idx(json_object_object_get(json_start_line,"stop_station"),i_station),"station_code")));
			json_transfer_one=transfer_one(code_start,end,msg);
			if(json_object_get_int(json_object_object_get(json_transfer_one,"transfer_count"))==-1){
				json_object_put(json_transfer_one);
				continue;
			}
			else{
				json_object_object_add(json_ret,"transfer_count",json_object_new_int(2));
				json_solution=json_object_new_object();
				json_object_object_add(json_solution,"t1",json_object_new_string(code_start));
				sprintf(condition,"%s|%s|%s|%s",rjultostrfmt(today,day+1,YEAR_TO_DAY),run_no_start,start,code_start);
				json_object_object_add(json_solution,"s_t1",get_direct_lines(condition));
				json_object_object_add(json_solution,"t1_e",json_transfer_one);
				json_object_array_add(json_object_object_get(json_ret,"solutions"),json_solution);
			}
		}
	}
	tpfree(rcvbuf);
	tpfree(sendbuf);
	json_object_put(json_start);
	return json_ret;
}

JSON_OBJECT transfer_three(char *start,char *end,char *msg)
{
	JSON_OBJECT json_start;
	JSON_OBJECT json_start_line,json_transfer_two;
	JSON_OBJECT json_ret,json_solution;
	char *sendbuf, *rcvbuf;
	int day;
	char today[30];
	char code_start[20],run_no_start[10],run_no_end[10];
	char condition[200];
    int ret;
	long sendlen,rcvlen;
	int i,i_station;

	/*initialize the return value*/
	json_ret=json_object_new_object();
	json_object_object_add(json_ret,"transfer_count",json_object_new_int(-1));
	json_object_object_add(json_ret,"solutions",json_object_new_array());

	day=rtoday();
	if((rcvbuf = (char *) tpalloc("CARRAY", NULL, 200)) == NULL) {
		strcpy(msg,"Error allocating send buffer\n");
		return json_ret;
	}
	if((sendbuf = (char *) tpalloc("CARRAY", NULL, 20000)) == NULL) {
		strcpy(msg,"Error allocating receive buffer\n");
		tpfree(rcvbuf);
		return json_ret;
	}
	
	/*obtain the trains departure from start station*/
	sprintf(sendbuf, "%s||%s||",rjultostrfmt(today,day+1,YEAR_TO_DAY),start);
	ret = tpcall("fdzquery", (char *)sendbuf, 1+strlen(sendbuf), (char **)&rcvbuf, &rcvlen, (long)0);

	if(ret){
		strcpy(msg,"tpcall error!");
		tpfree(sendbuf);
		tpfree(rcvbuf);
		return json_ret;
	}
	json_start=json_tokener_parse(rcvbuf);	
	for(i=0;i<json_object_array_length(json_start);i++){
		json_start_line=json_object_array_get_idx(json_start,i);
		strcpy(run_no_start,json_object_get_string(json_object_object_get(json_object_array_get_idx(json_object_object_get(json_start_line,"stop_station"),0),"run_train_no")));
		/*从出发站的下一站开始查询，i_station=1*/
		for(i_station=1;i_station<json_object_array_length(json_object_object_get(json_start_line,"stop_station"));i_station++){
			strcpy(code_start,json_object_get_string(json_object_object_get(json_object_array_get_idx(json_object_object_get(json_start_line,"stop_station"),i_station),"station_code")));
			json_transfer_two=transfer_two(code_start,end,msg);
			if(json_object_get_int(json_object_object_get(json_transfer_two,"transfer_count"))==-1){
				json_object_put(json_transfer_two);
				continue;
			}
			else{
				json_object_object_add(json_ret,"transfer_count",json_object_new_int(3));
				json_solution=json_object_new_object();
				json_object_object_add(json_solution,"t1",json_object_new_string(code_start));
				sprintf(condition,"%s|%s|%s|%s",rjultostrfmt(today,day+1,YEAR_TO_DAY),run_no_start,start,code_start);
				json_object_object_add(json_solution,"s_t1",get_direct_lines(condition));
				json_object_object_add(json_solution,"t1_e",json_transfer_two);
				json_object_array_add(json_object_object_get(json_ret,"solutions"),json_solution);
			}
		}
	}
	tpfree(rcvbuf);
	tpfree(sendbuf);
	json_object_put(json_start);
	return json_ret;
}

JSON_OBJECT transfer_four(char *start,char *end,char *msg)
{
	JSON_OBJECT json_start;
	JSON_OBJECT json_start_line,json_transfer_three;
	JSON_OBJECT json_ret,json_solution;
	char *sendbuf, *rcvbuf;
	int day;
	char today[30];
	char code_start[20],run_no_start[10],run_no_end[10];
	char condition[200];
    int ret;
	long sendlen,rcvlen;
	int i,i_station;

	/*initialize the return value*/
	json_ret=json_object_new_object();
	json_object_object_add(json_ret,"transfer_count",json_object_new_int(-1));
	json_object_object_add(json_ret,"solutions",json_object_new_array());

	day=rtoday();
	if((rcvbuf = (char *) tpalloc("CARRAY", NULL, 200)) == NULL) {
		strcpy(msg,"Error allocating send buffer\n");
		return json_ret;
	}
	if((sendbuf = (char *) tpalloc("CARRAY", NULL, 20000)) == NULL) {
		strcpy(msg,"Error allocating receive buffer\n");
		tpfree(rcvbuf);
		return json_ret;
	}
	
	/*obtain the trains departure from start station*/
	sprintf(sendbuf, "%s||%s||",rjultostrfmt(today,day+1,YEAR_TO_DAY),start);
	ret = tpcall("fdzquery", (char *)sendbuf, 1+strlen(sendbuf), (char **)&rcvbuf, &rcvlen, (long)0);
	if(ret){
		strcpy(msg,"tpcall error!");
		tpfree(sendbuf);
		tpfree(rcvbuf);
		return json_ret;
	}
	json_start=json_tokener_parse(rcvbuf);	

	for(i=0;i<json_object_array_length(json_start);i++){
		json_start_line=json_object_array_get_idx(json_start,i);
		strcpy(run_no_start,json_object_get_string(json_object_object_get(json_object_array_get_idx(json_object_object_get(json_start_line,"stop_station"),0),"run_train_no")));
		/*从出发站的下一站开始查询，i_station=1*/
		for(i_station=1;i_station<json_object_array_length(json_object_object_get(json_start_line,"stop_station"));i_station++){
			strcpy(code_start,json_object_get_string(json_object_object_get(json_object_array_get_idx(json_object_object_get(json_start_line,"stop_station"),i_station),"station_code")));
			json_transfer_three=transfer_three(code_start,end,msg);
			if(json_object_get_int(json_object_object_get(json_transfer_three,"transfer_count"))==-1){
				json_object_put(json_transfer_three);
				continue;
			}
			else{
				json_object_object_add(json_ret,"transfer_count",json_object_new_int(4));
				json_solution=json_object_new_object();
				json_object_object_add(json_solution,"t1",json_object_new_string(code_start));
				sprintf(condition,"%s|%s|%s|%s",rjultostrfmt(today,day+1,YEAR_TO_DAY),run_no_start,start,code_start);
				json_object_object_add(json_solution,"s_t1",get_direct_lines(condition));
				json_object_object_add(json_solution,"t1_e",json_transfer_three);
				json_object_array_add(json_object_object_get(json_ret,"solutions"),json_solution);
			}
		}
	}
	tpfree(rcvbuf);
	tpfree(sendbuf);
	json_object_put(json_start);
	return json_ret;
}

JSON_OBJECT train_transfer(char * start ,char * end,char *msg)
{
//	 FDZquery_stu fdz;
//	 char *sendbuf, *rcvbuf;
//	 int day;
//	 int ret;
//	 long sendlen,rcvlen;
	 JSON_OBJECT json,json_ret;
//	 day=rtoday();

	 json_ret=json_object_new_object();
	 json_object_object_add(json_ret,"transfer_count",json_object_new_int(-1));
			  
	 
//	 /* Attach to System/T as a Client Process */
//			         
//     /* Allocate CARRAY buffers for the request and the reply */
//     if((rcvbuf = (char *) tpalloc("CARRAY", NULL, 200)) == NULL) {
//         strcpy(msg,"Error allocating send buffer\n");
//         return json_ret;
//     }
//     if((sendbuf = (char *) tpalloc("CARRAY", NULL, 20000)) == NULL) {
//        strcpy(msg,"Error allocating send buffer\n");
//		tpfree(rcvbuf);
//        return json_ret;
//     }   
//	 
    /*First,check whether there is a train can get the end directly*/																										 
//	rjultostrfmt(fdz.on_date,day+1,YEAR_TO_DAY);
//    strcpy(fdz.begin,start);
//    strcpy(fdz.arrive,end);
//    *fdz.run_train_no=0;
//    json=json_object_new_object();
//    struct_to_json(json,&fdz,FDZquery_tpl,0);
//    strcpy(sendbuf,json_object_to_json_string(json));   
//	json_object_put(json);
//	sendlen = strlen(sendbuf)+1;
//    ret = tpcall("fdzquery", (char *)sendbuf, sendlen, (char **)&rcvbuf, &rcvlen, (long)0);

//	if(!ret){
//		json=json_tokener_parse(rcvbuf);
//		if(json_object_array_length(json)>0){
//			strcpy(msg,"there is at least one train to the end directly!");
//			tpfree(sendbuf);
//			tpfree(rcvbuf);
//			json_object_object_add(json_ret,"transfer_count",json_object_new_int(0));
//			json_object_object_add(json_ret,"solutions",json);
//			return json_ret;
//		}
//		else{
//			/*no direct train to the end ,check whether transfer onece can be done*/
//			json_object_put(json);
//			tpfree(sendbuf);
//		    tpfree(rcvbuf);
//		    printf("can't get with 0 transfer!\n");	
//			
//			json_object_put(json_ret);
//			json_ret=transfer_one(start,end,msg);
//			if(json_object_get_int(json_object_object_get(json_ret,"transfer_count"))==1){
//				strcpy(msg,"you can  get to the end with 1 transfer.");
//				return json_ret;
//			}
//			else{
//			/*try whether transfer twice can be done*/
//		    printf("can't get with 1 transfer!\n");	
//				json_object_put(json_ret);
//				json_ret=transfer_two(start,end,msg);
//				if(json_object_get_int(json_object_object_get(json_ret,"transfer_count"))==2){
//					strcpy(msg,"you can get to the end with 2 transfer.");
//					return json_ret;
//				}
//				else{
//					/*try whether transfer three times can be done*/
//		    		printf("can't get with 2 transfer!\n");	
//					json_object_put(json_ret);
//					json_ret=transfer_three(start,end,msg);
//					if(json_object_get_int(json_object_object_get(json_ret,"transfer_count"))==3){
//						strcpy(msg,"you can get to the end with 3 transfer.");
//						return json_ret;
//					}
//					else{
//						/*try whether transfer four times can be done*/
//		    			printf("can't get with 3 transfer!\n");	
//						json_object_put(json_ret);
//						json_ret=transfer_four(start,end,msg);
//						if(json_object_get_int(json_object_object_get(json_ret,"transfer_count"))==4){
//								strcpy(msg,"you can get to the end with 4 transfer.");
//								return json_ret;
//						}
//						else{
//							strcpy(msg,"you can't get to the end within 4 transfer.");
//		    				printf("can't get with 4 transfer!\n");	
//							return json_ret;
//						}
//					}
//				}
//				
//			}      
//		}
//	}
//	else{
//	    strcpy(msg,"tpcall error!") ;     
//		tpfree(sendbuf);
//		tpfree(rcvbuf);
//		return json_ret;
//		
//	}
		json_object_put(json_ret);
		json_ret=transfer_zero(start,end,msg);
		
		
		if(json_object_get_int(json_object_object_get(json_ret,"transfer_count"))==0){
			strcpy(msg,"you can get the end directly");
			return json_ret;
		}
		else{
			json_object_put(json_ret);
			json_ret=transfer_one(start,end,msg);
			if(json_object_get_int(json_object_object_get(json_ret,"transfer_count"))==1){
				strcpy(msg,"you can get the end with one transfer!");
				return json_ret;
			}
			else{
				json_object_put(json_ret);
				json_ret=transfer_two(start,end,msg);
				if(json_object_get_int(json_object_object_get(json_ret,"transfer_count"))==2){
					strcpy(msg,"you can get the end with two transfer!");
					return json_ret;
				}
				else{
					json_object_put(json_ret);
					json_ret=transfer_three(start,end,msg);
					if(json_object_get_int(json_object_object_get(json_ret,"transfer_count"))==3){
						strcpy(msg,"you can get the end with three transfer!");
						return json_ret;
					}
					else{
						json_object_put(json_ret);
						json_ret=transfer_one(start,end,msg);
						if(json_object_get_int(json_object_object_get(json_ret,"transfer_count"))==1){
							strcpy(msg,"you can get the end with one transfer!");
							return json_ret;
						}
						else{
							strcpy(msg,"you can't go there within four transfer!");
							return json_ret;
						}
					}
				}
			}
		}
		
			
}

/*解析并输出直达的车次信息*/
void transfer_parse_zero(JSON_OBJECT json)
{
	char run_train_no[10],start_station[100],end_station[100],from_station[100],to_station[100],on_time[30],off_time[30];
	JSON_OBJECT json_param,json_start,json_arrive,json_train;
	int i;
	
	printf("车次	始发站	出发站	出发时间	目的站	到达时间	终点站\n");
	
	for(i=0;i<json_object_array_length(json_object_object_get(json,"solutions"));i++){
	    
		json_param=json_object_object_get(json_object_array_get_idx(json_object_object_get(json,"solutions"),i),"param");
		json_start=json_object_object_get(json_object_array_get_idx(json_object_object_get(json,"solutions"),i),"start_station");
		json_arrive=json_object_object_get(json_object_array_get_idx(json_object_object_get(json,"solutions"),i),"arrive_station");
		json_train=json_object_object_get(json_object_array_get_idx(json_object_object_get(json,"solutions"),i),"train");
		strcpy(run_train_no,json_object_get_string(json_object_object_get(json_start,"run_train_no")));
		
		strcpy(start_station,json_object_get_string(json_object_object_get(json_train,"start_station")));
		strcpy(end_station,json_object_get_string(json_object_object_get(json_train,"end_station")));
		strcpy(from_station,json_object_get_string(json_object_object_get(json_start,"station_code")));
		strcpy(to_station,json_object_get_string(json_object_object_get(json_arrive,"station_code")));
		strcpy(on_time,json_object_get_string(json_object_object_get(json_param,"on_time")));
		strcpy(off_time,json_object_get_string(json_object_object_get(json_param,"off_time")));
		printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\n",run_train_no,start_station,from_station,on_time,to_station,off_time,end_station);

	}
	
}
/*解析并输出换乘1次的车次信息*/
void transfer_parse_one(JSON_OBJECT json)
{
	char first_run_train_no[10],first_on_time[30],from_station[100],first_off_time[30],transfer_station[100];
	char second_run_train_no[10],second_on_time[30],to_station[100],second_off_time[30];
	JSON_OBJECT json_param_first,json_start_first,json_arrive_first;
	JSON_OBJECT json_param_second,json_start_second,json_arrive_second;
	int i;
	printf("初乘车次	出发时间		出发站	中转到达时间		中转站	中转车次	中转发车时间	目的站	终到站时间\n");
	for(i=0;i<json_object_array_length(json_object_object_get(json,"solutions"));i++){
		
		json_param_first=json_object_object_get(json_object_array_get_idx(json_object_object_get(json_object_array_get_idx(json_object_object_get(json,"solutions"),i),"s_t1"),0),"param");
		json_start_first=json_object_object_get(json_object_array_get_idx(json_object_object_get(json_object_array_get_idx(json_object_object_get(json,"solutions"),i),"s_t1"),0),"start_station");
		json_arrive_first=json_object_object_get(json_object_array_get_idx(json_object_object_get(json_object_array_get_idx(json_object_object_get(json,"solutions"),i),"s_t1"),0),"arrive_station");
		
		json_param_second=json_object_object_get(json_object_array_get_idx(json_object_object_get(json_object_array_get_idx(json_object_object_get(json,"solutions"),i),"t1_e"),0),"param");
		json_start_second=json_object_object_get(json_object_array_get_idx(json_object_object_get(json_object_array_get_idx(json_object_object_get(json,"solutions"),i),"t1_e"),0),"start_station");
		json_arrive_second=json_object_object_get(json_object_array_get_idx(json_object_object_get(json_object_array_get_idx(json_object_object_get(json,"solutions"),i),"t1_e"),0),"arrive_station");
		strcpy(transfer_station,json_object_get_string(json_object_object_get(json_object_array_get_idx(json_object_object_get(json,"solutions"),i),"t1")));

		strcpy(first_run_train_no,json_object_get_string(json_object_object_get(json_start_first,"run_train_no")));
		strcpy(first_on_time,json_object_get_string(json_object_object_get(json_param_first,"on_time")));
		strcpy(from_station,json_object_get_string(json_object_object_get(json_start_first,"station_code")));
		strcpy(first_off_time,json_object_get_string(json_object_object_get(json_param_first,"off_time")));

		strcpy(second_run_train_no,json_object_get_string(json_object_object_get(json_start_second,"run_train_no")));
		strcpy(second_on_time,json_object_get_string(json_object_object_get(json_param_second,"on_time")));
		strcpy(to_station,json_object_get_string(json_object_object_get(json_arrive_second,"station_code")));
		strcpy(second_off_time,json_object_get_string(json_object_object_get(json_param_second,"off_time")));

		printf("%s\t\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",first_run_train_no,first_on_time,from_station,first_off_time,transfer_station,second_run_train_no,second_on_time,to_station,second_off_time);
				
	}
}
/*解析并输出换乘2次的车次信息*/
void transfer_parse_two(JSON_OBJECT json)
{
}
/*解析并输出换乘3次的车次信息*/
void transfer_parse_three(JSON_OBJECT json)
{
}
/*解析并输出换乘4次的车次信息*/
void transfer_parse_four(JSON_OBJECT json)
{
}
