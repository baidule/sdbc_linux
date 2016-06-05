#include "tjsTPC.h"

//需要由应用者提供的模板库
extern T_PkgType *tpl_tab[];

#ifdef __cplusplus
extern "C"
#endif
T_PkgType * get_tpl(char *tabname)
{
T_PkgType *tp=NULL;
int i,n;
	for(i=0;tpl_tab[i];i++) {
		tp=tpl_tab[i];
		n=set_offset(tp);
		if(!tp[n].name || !*tp[n].name) continue;
		if(!strcmp(tp[n].name,tabname))
			break;
	}
	if(!tpl_tab[i]) return NULL;
	return tp;
}

#ifdef __cplusplus
extern "C"
#endif
char *mk_rec(T_PkgType *tp)
{
char *p;
int i;

	if(!tp) return NULL;
	i=set_offset(tp);
	p=malloc(tp[i].offset);
	return p;
}

