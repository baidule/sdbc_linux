#ifdef __cplusplus

//һ�������ڶ��̻߳�����sdbc���ӳأ��̳߳ػ����ļ�../mod_sc
extern "C" {
#endif

//rcpool.c
int rcpool_init(void);
void rcpool_free(void);
void rcpool_check(void);

T_Connect * get_RC_connect(int poolno,int flg);
void release_RC_connect(T_Connect **Connect,int poolno);
int get_rcpool_no(int d_node);
int get_rcpoolnum(void);
char *get_rcLABEL(int poolno);

#ifdef __cplusplus
}
#endif
