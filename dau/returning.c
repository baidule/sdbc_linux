/***********************************************************
 * INSERT INTO ...... RETURNING .... INTO .....
 ***********************************************************/
#include <SRM.h>

/* Éú³ÉRETURNING×Ó¾ä  */
char * SRM_mk_returning(SRM *srmp,const char *keys,char *stmt)
{
char pks[104],*p,*p1,*p2,*p3,into[256];
int i,n; 

	if(!srmp||!stmt) return 0;
	if(!keys || !*keys) {
		*stmt=0;
		return stmt;
	}
	p1=stmt;
	*p1=0;
	p=(char *)keys;
	p3=into;
	for(i=0;*p;i++) {
		p=stptok(skipblk(p),pks,sizeof(pks),",|");
		if(*p) p++;
		if(-1==(n=index_col(srmp->colidx,abs(srmp->Aflg),pks,srmp->tp)))
			continue;
		srmp->tp[n].bindtype |= NOINS|RETURNING;
        	if(i==0) {
			p1=stpcpy(p1," RETURNING ");
			p3=stpcpy(p3," INTO :");
		} else {
			p1=stpcpy(p1,", ");
			p3=stpcpy(p3,", :");
		}
		p2=(char*)srmp->tp[n].name;
		switch(srmp->tp[n].type) {
		case CH_DATE:
		case CH_JUL:
		case CH_MINUTS:
		case CH_TIME:
		case CH_USEC:
			p1=stpcpy(ext_copy(stpcpy(strtcpy(stpcpy(p1,"TO_CHAR("),&p2,' '),",'"),srmp->tp[n].format),"')");
			break;
		case CH_CNUM:
			p1=stpcpy(strtcpy(stpcpy(p1,"TO_CHAR("),&p2,' '),")");
			break;
		default:
                	p1=strtcpy(p1,&p2,' ');
			break;
		}
		p3=stpcpy(p3,pks);

	}
	p1=stpcpy(p1,into);
	return p1;
}

