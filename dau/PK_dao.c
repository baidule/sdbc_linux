#include <DAU.h>

char * mk_PK(DAU *DP,char *stmt)
{
char *p,*wp=0;

	if(*stmt) wp=strdup(stmt);
	p=mk_where(DP->srm.pks,stmt);
	if(!*stmt) {
		if(wp) free(wp);
		sprintf(DP->SQL_Connect->ErrMsg,"PK not exist!");
		DP->SQL_Connect->Errno=FORMATERR;
		return NULL;
	}
	if(wp) {
		p=stpcpy(p,wp);
		free(wp);
	}
	return p;
}

int select_by_PK(DAU *DP,char *stmt)
{
int ret;

	if(!mk_PK(DP,stmt)) return FORMATERR;
	ret=DAU_select(DP,stmt,1);
	if(ret<0) return ret;
	if(ret==0) return -1;
	return DAU_next(DP);
}

int prepare_by_PK(DAU *DP,char *stmt)
{
int ret;

	if(DP->cursor<0) {
		if(!mk_PK(DP,stmt)) return FORMATERR;
	}
	ret=DAU_prepare(DP,stmt);
	if(ret) {
		return ret;
	}
	return DAU_next(DP);
}

int update_by_PK(DAU *DP,char *stmt)
{
	if(DP->upd_sth<0) {
		if(!mk_PK(DP,stmt)) return FORMATERR;
	}
	return DAU_update(DP,stmt);
}

int delete_by_PK(DAU *DP,char *stmt)
{
	if(DP->del_sth<0) {
		if(!mk_PK(DP,stmt)) return FORMATERR;
	}
	return DAU_delete(DP,stmt);
}

int dummy_update(DAU *DP,char *stmt)
{

	if(DP->upd_sth < 0) {
	char col_name[83],*p1;
	char *p=DAU_mk_update(DP,stmt);
		p1=(char *)DP->srm.tp[0].name;
		strtcpy(col_name,&p1,' ');
		
		p+=sprintf(p,"SET %s=%s ",col_name,col_name);
		p=mk_where(DP->srm.pks,p);
	}
	return DAU_update(DP,stmt);
}
