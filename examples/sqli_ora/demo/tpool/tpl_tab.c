#include "tjsTPC.h"

T_PkgType TI_CITY_tpl[]={
        {CH_CHAR,5,"CITY_CODE",0,-1},
        {CH_CHAR,21,"CITY_NAME"},
        {CH_CHAR,4,"CITY_SHORTCODE"},
        {CH_CHAR,2,"CITY_CLASS"},
        {CH_CHAR,6,"DAIL_AREA_CODE"},
        {CH_JUL,sizeof(INT4),"CITY_START_DATE","YYYYMMDD"},
        {CH_JUL,sizeof(INT4),"CITY_STOP_DATE","YYYYMMDD"},
        {CH_CHAR,2,"DISTRICT_CODE"},
        {CH_CHAR,3,"PROVINCE_CODE"},
        {-1,0,"TI_CITY","CITY_CODE|"}
};

T_PkgType *tpl_tab[]={
	TI_CITY_tpl,
	STATION_tpl,
	TRAIN_tpl,
	STOP_STATION_tpl,
	NULL
};
