#include <DAU.h>

int SRM_to_DAU(DAU *DP,T_SQL_Connect *SQL_Connect,SRM *srmp)
{
	DP->srm = *srmp;
 	DP->SQL_Connect=SQL_Connect;
    DP->cursor=SQLO_STH_INIT;
    DP->ins_sth=SQLO_STH_INIT;
    DP->upd_sth=SQLO_STH_INIT;
	DP->del_sth=SQLO_STH_INIT;
    DP->bt_pre=0;
	DP->bt_ins=0;
    DP->bt_upd=0;
	DP->bt_del=0;
	return 0;
}

int DAU_to_SRM(DAU *DP,SRM *srmp)
{
	*srmp = DP->srm;
	 if(DP->cursor >= 0) {
        ___SQL_Close__(DP->SQL_Connect,DP->cursor);
        DP->cursor=SQLO_STH_INIT;
    	BB_Tree_Free(&DP->bt_pre,0);
    }
    if(DP->ins_sth >= 0) {
        ___SQL_Close__(DP->SQL_Connect,DP->ins_sth);
        DP->ins_sth=SQLO_STH_INIT;
    	BB_Tree_Free(&DP->bt_ins,0);
    }
    if(DP->upd_sth >= 0) {
        ___SQL_Close__(DP->SQL_Connect,DP->upd_sth);
        DP->upd_sth=SQLO_STH_INIT;
    	BB_Tree_Free(&DP->bt_upd,0);
    }
    if(DP->del_sth >= 0) {
        ___SQL_Close__(DP->SQL_Connect,DP->del_sth);
        DP->del_sth=SQLO_STH_INIT;
    	BB_Tree_Free(&DP->bt_del,0);
    }
	return 0;
}

int SRM_next(SRM *srmp)
{
	if(!srmp->rp || !*srmp->rp) return -1;
	srmp->rp+=net_dispack(srmp->rec,srmp->rp,srmp->tp);
	if(!*srmp->rp) {
		free(srmp->result);
		srmp->result=0;
		srmp->rp=0;
	}
	return 0;
}

