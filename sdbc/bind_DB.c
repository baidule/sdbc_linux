/******************************************************
 * ���mod_DB,����ȡ�����ݿ����Ӻ󣬴��ݸ���Ӧ��TCB,
 * �����Ӧ��TCB�г�ȥ֮�� ���Ա�����Ӧ����������
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

