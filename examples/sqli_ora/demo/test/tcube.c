/**********************************************************
 * test CUBE  with ORACLE's  
 **********************************************************/

#include <DAU.h>
#include <sys/time.h>
#include <tree.h>
extern T_Tree *Tree_Add();
struct t_node {
	char unit[9];
	char tabname[9];
	short flg;
	int count;
	INT64 s;
	double ag;
	int *num;
};

static int cmp(void *t,void *s,int len)
{
struct t_node *tp,*sp;
int ret;

	tp=(struct t_node *)t;
	sp=(struct t_node *)s;
	ret=strcmp(tp->unit,sp->unit);
	if(ret) return ret;
	ret=strcmp(tp->tabname,sp->tabname);
	if(ret) return ret;
	if(tp->flg > sp->flg) return 1;
	if(tp->flg < sp->flg) return -1;
	return 0;
}

static int add_dup(T_Tree *t,void *s,int len)
{
struct t_node *tp,*sp;
int ret;

	tp=(struct t_node *)t->Content;
	sp=(struct t_node *)s;
	if(sp->s != INT64NULL) {
		if(tp->s == INT64NULL) tp->s=sp->s;
		else {
			tp->count++;
			tp->s+=sp->s;
		}
	}
	return 0;
}

static int avg(void *t)
{
struct t_node *tp;

	tp=(struct t_node *)t;
	if(tp->s != INT64NULL) tp->ag = (double)tp->s/tp->count;
	return 0;
}

static int print_node(void *t)
{
struct t_node *tp;
char buf[10],s[20];

	tp=(struct t_node *)t;
	(*tp->num)++;
	*buf=0;
	if(tp->flg != SHORTNULL) itoStr(tp->flg,buf);
	*s=0;
	if(tp->s != INT64NULL) lltoStr(tp->s,s);
	printf("%s,%s,%s:%d,%s,%.2lf\n",
		tp->unit,tp->tabname,buf,
		tp->count,s,tp->ag);
	return 0;
}

T_PkgType cube_tpl[]={
        {CH_CHAR,9,"unit",0,-1},
        {CH_CHAR,9,"tabname"},
        {CH_SHORT,sizeof(short),"flg"},
        {CH_INT,sizeof(int),"count(*) count"},
        {CH_INT64,sizeof(INT64),"sum(dat1) s"},
        {CH_DOUBLE,sizeof(double),"avg(dat1) ag","%.2lf"},
        {-1,0,"tjrb","tjdate|unit|tabname|flg|"}
};

extern T_PkgType cube_tpl[];
typedef struct {
        char unit[9];
        char tabname[9];
        short flg;
        int count;
        INT64 s;
        double ag;
} cube_stu;


T_PkgType TJRB_tpl[]={
        {CH_DATE,YEAR_TO_DAY_LEN,"tjdate",YEAR_TO_DAY,-1},
        {CH_CHAR,9,"unit"},
        {CH_CHAR,9,"tabname"},
        {CH_SHORT,sizeof(short),"flg"},
        {CH_INT64,sizeof(INT64),"dat1"},
        {CH_INT64,sizeof(INT64),"dat2"},
        {CH_INT64,sizeof(INT64),"dat3"},
        {CH_INT64,sizeof(INT64),"dat4"},
        {CH_INT64,sizeof(INT64),"dat5"},
        {CH_INT64,sizeof(INT64),"dat6"},
        {CH_INT64,sizeof(INT64),"dat7"},
        {CH_INT64,sizeof(INT64),"dat8"},
        {CH_INT64,sizeof(INT64),"dat9"},
        {CH_INT64,sizeof(INT64),"dat10"},
        {CH_INT64,sizeof(INT64),"dat11"},
        {CH_INT64,sizeof(INT64),"dat12"},
        {CH_INT64,sizeof(INT64),"dat13"},
        {CH_INT64,sizeof(INT64),"dat14"},
        {CH_INT64,sizeof(INT64),"dat15"},
        {CH_INT64,sizeof(INT64),"dat16"},
        {CH_INT64,sizeof(INT64),"dat17"},
        {CH_INT64,sizeof(INT64),"dat18"},
        {CH_INT64,sizeof(INT64),"dat19"},
        {CH_INT64,sizeof(INT64),"dat20"},
        {CH_INT64,sizeof(INT64),"dat21"},
        {CH_INT64,sizeof(INT64),"dat22"},
        {CH_INT64,sizeof(INT64),"dat23"},
        {CH_INT64,sizeof(INT64),"dat24"},
        {CH_INT64,sizeof(INT64),"dat25"},
        {CH_INT64,sizeof(INT64),"dat26"},
        {CH_INT64,sizeof(INT64),"dat27"},
        {CH_INT64,sizeof(INT64),"dat28"},
        {CH_INT64,sizeof(INT64),"dat29"},
        {CH_INT64,sizeof(INT64),"dat30"},
        {CH_INT64,sizeof(INT64),"dat31"},
        {CH_INT64,sizeof(INT64),"dat32"},
        {CH_INT64,sizeof(INT64),"dat33"},
        {CH_INT64,sizeof(INT64),"dat34"},
        {CH_INT64,sizeof(INT64),"dat35"},
        {CH_INT64,sizeof(INT64),"dat36"},
        {CH_INT64,sizeof(INT64),"dat37"},
        {CH_INT64,sizeof(INT64),"dat38"},
        {CH_INT64,sizeof(INT64),"dat39"},
        {CH_INT64,sizeof(INT64),"dat40"},
        {CH_INT64,sizeof(INT64),"dat41"},
        {CH_INT64,sizeof(INT64),"dat42"},
        {CH_INT64,sizeof(INT64),"dat43"},
        {CH_INT64,sizeof(INT64),"dat44"},
        {CH_INT64,sizeof(INT64),"dat45"},
        {CH_INT64,sizeof(INT64),"dat46"},
        {CH_INT64,sizeof(INT64),"dat47"},
        {CH_INT64,sizeof(INT64),"dat48"},
        {CH_INT64,sizeof(INT64),"dat49"},
        {CH_INT64,sizeof(INT64),"dat50"},
        {-1,0,"TJRB","tjdate|unit|tabname|flg|"}
};

extern T_PkgType TJRB_tpl[];
typedef struct {
        char tjdate[YEAR_TO_DAY_LEN];
        char unit[9];
        char tabname[9];
        short flg;
        INT64 dat1;
        INT64 dat2;
        INT64 dat3;
        INT64 dat4;
        INT64 dat5;
        INT64 dat6;
        INT64 dat7;
        INT64 dat8;
        INT64 dat9;
        INT64 dat10;
        INT64 dat11;
        INT64 dat12;
        INT64 dat13;
        INT64 dat14;
        INT64 dat15;
        INT64 dat16;
        INT64 dat17;
        INT64 dat18;
        INT64 dat19;
        INT64 dat20;
        INT64 dat21;
        INT64 dat22;
        INT64 dat23;
        INT64 dat24;
        INT64 dat25;
        INT64 dat26;
        INT64 dat27;
        INT64 dat28;
        INT64 dat29;
        INT64 dat30;
        INT64 dat31;
        INT64 dat32;
        INT64 dat33;
        INT64 dat34;
        INT64 dat35;
        INT64 dat36;
        INT64 dat37;
        INT64 dat38;
        INT64 dat39;
        INT64 dat40;
        INT64 dat41;
        INT64 dat42;
        INT64 dat43;
        INT64 dat44;
        INT64 dat45;
        INT64 dat46;
        INT64 dat47;
        INT64 dat48;
        INT64 dat49;
        INT64 dat50;
} TJRB_stu;


main(int argc,char **argv)
{
cube_stu cube;
TJRB_stu tjrb;
DAU tjrb_DAU,cube_DAU;
T_SQL_Connect SQL_Connect;
int ret,num=0;
char stmt[4096];
INT64 now;
T_PkgType tj_tpl[5];
struct t_node node;
T_Tree *root=0;

	if(argc>1) ret=envcfg(argv[1]);
	ret=db_open(&SQL_Connect);
	if(ret) {
		printf("%s,dbopen error\n",argv[0]);
		return 1;
	}
	patt_copy(tj_tpl,TJRB_tpl,"unit,tabname,flg,dat1");
	DAU_init(&tjrb_DAU,&SQL_Connect,0,&tjrb,tj_tpl);
//测试树group
	node.num=&num;
	now=now_usec();
	*stmt=0;
	ret=DAU_prepare(&tjrb_DAU,stmt);
	ShowLog(5,"%s:stmt=%s",__FUNCTION__,stmt);
	if(ret!=0) {
		printf("%s,err=%d,%s\n",stmt,
			SQL_Connect.Errno,
			SQL_Connect.ErrMsg);
	} else {
		while(!DAU_next(&tjrb_DAU)) {
			node.count=1;
			node.s=tjrb.dat1;
			*node.unit=0;
			*node.tabname=0;
			node.flg=SHORTNULL;
			root=Tree_Add(root,&node,sizeof(node),cmp,add_dup); //000
			node.flg=tjrb.flg;	
			root=Tree_Add(root,&node,sizeof(node),cmp,add_dup); //001
			strcpy(node.tabname,tjrb.tabname);
			root=Tree_Add(root,&node,sizeof(node),cmp,add_dup); //011
			node.flg=SHORTNULL;
			root=Tree_Add(root,&node,sizeof(node),cmp,add_dup); //010
			strcpy(node.unit,tjrb.unit);
			root=Tree_Add(root,&node,sizeof(node),cmp,add_dup); //110
			node.flg=tjrb.flg;	
			root=Tree_Add(root,&node,sizeof(node),cmp,add_dup); //111
			*node.tabname=0;
			root=Tree_Add(root,&node,sizeof(node),cmp,add_dup); //101
			node.flg=SHORTNULL;
			root=Tree_Add(root,&node,sizeof(node),cmp,add_dup); //100
		}
		if(root) {
			BB_Tree_Scan(root,avg);
			BB_Tree_Scan(root,print_node);
			BB_Tree_Free(&root,0);
		}
//		printf("//cube,不打印结果\n");
		printf("Cube by Tree:num=%d,TIMEVAL=%lld\n",num,now_usec()-now);
}
	DAU_free(&tjrb_DAU);
//测试CUBE
	num=0;
	DAU_init(&cube_DAU,&SQL_Connect,0,&cube,cube_tpl);
	now=now_usec();
	strcpy(stmt,"GROUP BY CUBE (unit,tabname,flg)");
	ret=DAU_prepare(&cube_DAU,stmt);
	ShowLog(5,"%s:stmt=%s",__FUNCTION__,stmt);
	if(ret!=0) {
		printf("%s,err=%d,%s\n",stmt,
			SQL_Connect.Errno,
			SQL_Connect.ErrMsg);
	} else {
		while(!DAU_next(&cube_DAU)) {
			num++;
			DAU_pack(&cube_DAU,stmt);
			printf("%s\n",stmt);
		}
	}
	printf("Group by cube:num=%d,TIMEVAL=%lld\n",num,now_usec()-now);

	DAU_free(&cube_DAU);

	sprintf(stmt,"select utl_raw.cast_to_varchar2 (utl_raw.cast_to_raw( chr(130) )) from dual");
	char *rec=0;
	ret=___SQL_Select__(&SQL_Connect,stmt,&rec,0);
	if(ret<=0) {
		printf("chr(130) err=%d,%s\n",SQL_Connect.Errno,SQL_Connect.ErrMsg);
	} else {
		printf("rec=%s,len=%d,%02X\n",rec,strlen(rec),255&*rec);
	}
	if(rec) free(rec);
	___SQL_CloseDatabase__(&SQL_Connect);
	return 0;
}
