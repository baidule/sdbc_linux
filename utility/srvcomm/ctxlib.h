/***************************************************
 * Զ����֤���� SDBC 7.0
 * �����ڵ���������������Զ����֤
 * �������֤��ϢӦ��¼�����ݿ���
 * *************************************************/

#include <pack.h>
#include <ctx.stu>

#ifdef __cplusplus
extern "C" {
#endif

void ctx_check(void);
CTX_stu * get_ctx(int ctx_id);
int set_ctx(CTX_stu *ctxp);
int ctx_del(CTX_stu *ctxp);
void ctx_free(void);

#ifdef __cplusplus
}
#endif
