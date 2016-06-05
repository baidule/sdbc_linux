#include <DAU.h>

//JSON_OBJECT get_price(T_SQL_Connect *SQL_Connect,int id,char *start,char *end,char *date)
JSON_OBJECT get_price(T_SQL_Connect *SQL_Connect,JSON_OBJECT input)
{
DAU t_price_DAU;
char stmt[4096];
TRAIN_PRICE_stu price;
JSON_OBJECT json;
char *p;

	DAU_init(&t_price_DAU,SQL_Connect,0,&price,TRAIN_PRICE_tpl);
/*
	strcpy(price.start_station,start);
	strcpy(price.end_station,end);
	strcpy(price.end_date,date);
	price.id=id;
*/
	DAU_fromJSON(&t_price_DAU,input);

//	sprintf(stmt,"where id=:id AND start_station=:start_station and end_station=:end_station and end_date >= :end_date)");
	p=DAU_mk_where(&t_price_DAU,"id,start_station,end_station",stmt);
	p+=sprintf(p," and end_date >= :end_date");
	ret=DAU_select(t_price_DAU,stmt,1);

	ret=DAU_next(&T_price_DAU);
	
	json=json_object_new_object();
	DAU_tiJSON(&t_price_DAU,json,0);
	return json;
}
