/******************************************************
 * 配合mod_DB,用于取得数据库连接后，传递给相应的TCB,
 * 或从相应的TCB中除去之。 可以被其他应用需求重载
 *****************************************************/
#include <sdbc.h>

int bind_DB(int TCBno,T_SQL_Connect *sql)
{
T_SRV_Var *srvp;

        srvp=get_SRV_Var(TCBno);
        if(srvp==NULL) return -1;
        srvp->SQL_Connect=sql;
        return 0;
}
int unbind_DB(int TCBno)
{
T_SRV_Var *srvp;

        srvp=get_SRV_Var(TCBno);
        if(srvp==NULL) return -1;
        srvp->SQL_Connect=NULL;
        return 0;
}

