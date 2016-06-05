#include "tpoolsrv.h"
void set_showid(void *ctx)
{
GDA *gp=(GDA *)ctx;
pthread_t tid=pthread_self();
	if(!ctx) return;
	mthr_showid_add(tid,gp->ShowID);
}
