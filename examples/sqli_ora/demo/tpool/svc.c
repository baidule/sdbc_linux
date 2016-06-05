#include <tjs.h>

int hello_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead,hello_app); 
}

int svc_price(T_Connect *conn,T_NetHead *nethead)
{
INT64 now=now_usec();
 	int ret = json_svc(conn,nethead,app_price); 
	ShowLog(5,"%s:TIMEVAL=%d",__FUNCTION__,INTERVAL);
	return ret;
}

int reload_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead,reload_app); 
}


// tangxw
/*
int te_ts_sp_astar_radix_multi_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead, te_ts_sp_Astar_radix_multi_app); 
}
*/
/* //2011.3.15
int te_ts_sp_radix_multi_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead,te_ts_sp_radix_multi_app); 
}
int te_tl_sp_radix_multi_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead, te_tl_sp_radix_multi_app); 
}*/

int te_ts_ksp_radix_multi_svc(T_Connect *conn,T_NetHead *nethead)
{
//INT64 now=now_usec();
int ret=json_svc(conn,nethead, te_ts_ksp_radix_multi_app); 
//	ShowLog(5,"%s:TIMEVAL=%d",__FUNCTION__,INTERVAL);
 	return ret;
}
int bus_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead,bus_app); 
}
int flight_train_bus_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead,flight_train_bus_app); 
}
int te_ts_ksp_radix_multi_flight_svc(T_Connect *conn,T_NetHead *nethead)
{
//INT64 now=now_usec();
int ret=json_svc(conn,nethead, te_ts_ksp_radix_multi_flight_app);
//      ShowLog(5,"%s:TIMEVAL=%d",__FUNCTION__,INTERVAL);
        return ret;
}
int te_ts_ksp_radix_multi_train_svc(T_Connect *conn,T_NetHead *nethead)
{
//INT64 now=now_usec();
int ret=json_svc(conn,nethead, te_ts_ksp_radix_multi_train_app);
//      ShowLog(5,"%s:TIMEVAL=%d",__FUNCTION__,INTERVAL);
        return ret;
}

int te_tl_ksp_radix_multi_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead, te_tl_ksp_radix_multi_app); 
}
int te_tl_ksp_radix_multi_flight_svc(T_Connect *conn,T_NetHead *nethead)
{
        return json_svc(conn,nethead, te_tl_ksp_radix_multi_flight_app);
}
int te_tl_ksp_radix_multi_train_svc(T_Connect *conn,T_NetHead *nethead)
{
        return json_svc(conn,nethead, te_tl_ksp_radix_multi_train_app);
}

int checi_query_svc(T_Connect *conn,T_NetHead *nethead)
{
	return json_svc(conn,nethead,checi_query_app);
}
int station_query_svc(T_Connect *conn,T_NetHead *nethead)
{
	return json_svc(conn,nethead,station_query_app);
}
int station_query_flight_svc(T_Connect *conn,T_NetHead *nethead)
{
	return json_svc(conn,nethead,station_query_flight_app);
}
int station_query_train_svc(T_Connect *conn,T_NetHead *nethead)
{
	return json_svc(conn,nethead,station_query_train_app);
}
int station_passby_svc(T_Connect *conn,T_NetHead *nethead)
{
	return json_svc(conn,nethead,station_passby_app);
}
int te_ts_ksp_radix_multi_avoid_svc(T_Connect *conn,T_NetHead *nethead)
{
	return json_svc(conn,nethead,te_ts_ksp_radix_multi_avoid_app);
}
int te_tl_ksp_radix_multi_avoid_svc(T_Connect *conn,T_NetHead *nethead)
{
	return json_svc(conn,nethead,te_tl_ksp_radix_multi_avoid_app);
}
int te_ts_ksp_radix_multi_avoid_flight_svc(T_Connect *conn,T_NetHead *nethead)
{
	return json_svc(conn,nethead,te_ts_ksp_radix_multi_avoid_flight_app);
}
int te_tl_ksp_radix_multi_avoid_flight_svc(T_Connect *conn,T_NetHead *nethead)
{
	return json_svc(conn,nethead,te_tl_ksp_radix_multi_avoid_flight_app);
}
int te_ts_ksp_radix_multi_avoid_train_svc(T_Connect *conn,T_NetHead *nethead)
{
	return json_svc(conn,nethead,te_ts_ksp_radix_multi_avoid_train_app);
}
int te_tl_ksp_radix_multi_avoid_train_svc(T_Connect *conn,T_NetHead *nethead)
{
	return json_svc(conn,nethead,te_tl_ksp_radix_multi_avoid_train_app);
}

int radix_init_svc(T_Connect* conn,T_NetHead *nethead)
{
	return json_svc(conn,nethead,radix_init_app);
}
int radix_update_svc(T_Connect* conn,T_NetHead *nethead)
{
	return json_svc(conn,nethead,radix_update_app);
}
//edited by ql
int te_tl_ksp_radix_multi_price_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead, te_tl_ksp_radix_multi_price_app); 
}
int te_tl_ksp_radix_multi_train_price_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead, te_tl_ksp_radix_multi_train_price_app); 
}
int te_tl_ksp_radix_multi_flight_price_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead, te_tl_ksp_radix_multi_flight_price_app); 
}

// chenshuiping
int te_ae_ksp_astar_heap_multi_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead,te_ae_ksp_astar_heap_multi_app); 
}
int te_ae_ksp_astar_heap_multi_train_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead,te_ae_ksp_astar_heap_multi_train_app); 
}
int te_ae_ksp_astar_heap_multi_flight_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead,te_ae_ksp_astar_heap_multi_flight_app); 
}
/*int te_ae_sp_astar_heap_single_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead,te_ae_sp_astar_heap_single_app); 
}
// jinwen
int td_ae_sp_astar_heap_single_svc(T_Connect *conn,T_NetHead *nethead)
{
 	return json_svc(conn,nethead,td_ae_sp_Astar_heap_single_app); 
}
*/
// qinlong

int td_ae_ksp_astar_buckets_multi_svc(T_Connect *conn,T_NetHead *nethead)
{
        return json_svc(conn,nethead,td_ae_ksp_astar_buckets_multi_app);
}

int td_tl_ksp_di_buckets_multi_svc(T_Connect *conn,T_NetHead *nethead)
{
        return json_svc(conn,nethead,td_tl_ksp_di_buckets_multi_app);
}

int td_lp_ksp_di_buckets_multi_svc(T_Connect *conn,T_NetHead *nethead)
{
        return json_svc(conn,nethead,td_lp_ksp_di_buckets_multi_app);
}
/*
//huxiaolin
int te_ts_sp_dij_buckets_single_svc(T_Connect *conn,T_NetHead *nethead)
{
        return json_svc(conn,nethead,te_ts_sp_dij_buckets_single_app);
}*/
int te_ts_sp_bidij_buckets_single_svc(T_Connect *conn,T_NetHead *nethead)
{
        return json_svc(conn,nethead,te_ts_sp_bidij_buckets_single_app);
}
/*
int te_ts_ksp_bidij_buckets_single_svc(T_Connect *conn,T_NetHead *nethead)
{
        return json_svc(conn,nethead,te_ts_ksp_bidij_buckets_single_app);
}
int te_ts_m2m_bidij_buckets_single_svc(T_Connect *conn,T_NetHead *nethead)
{
        return json_svc(conn,nethead,te_ts_m2m_bidij_buckets_single_app);
}*/
