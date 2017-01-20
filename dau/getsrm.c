/********************************************************************
 * 自动生成模板的程序
 * 根据数据库表结构生成模板。需要一个辅助的表：PATTERN_COL,
 * 对数据结构进行补充定义,这个表可以不存在或空，表示没有补充和修正
 * 如果对某表的某列有补充和修正，就在这个表中建立一条记录。
 * 本程序适用于ORACLE
 *** 寻找唯一索引的语句：
 select  TABLE_NAME,COLUMN_NAME,COLUMN_POSITION FROM all_ind_columns where table_name='SEAT' and
 index_name=(select index_name from all_indexes where table_name= 'SEAT' AND UNIQUENESS='UNIQUE' AND rownum < 2)
 order by COLUMN_POSITION;

 ********************************************************************/
#include <DAU.h>
#include <ctype.h>
#include "getsrm.h"

extern void PatternFree(SRM *srmp);

#include "descDAO.c"

#define THROW goto
char col_to_lower=0;
//如果该表没有主键，找一个唯一索引当作主键
T_PkgType ALL_IND_COLUMNS_tpl[]={
	{CH_CHAR,49,"TABLE_NAME",0,-1},
	{CH_CHAR,49,"COLUMN_NAME"},
	{CH_SHORT,sizeof(short),"COLUMN_POSITION"},
	{CH_CHAR,31,"INDEX_NAME"},
	{-1,0," ALL_IND_COLUMNS",0,0}
};

typedef struct {
	char TABLE_NAME[49];
	char COLUMN_NAME[49];
	short COLUMN_POSITION;
	char INDEX_NAME[31];
} ALL_IND_COLUMNS_stu;


//字符串中有小写
static int lcase(char *str)
{
	while(*str) {
		if(islower(*str)) return 1;
		str++;
	}
	return 0;
}

static char *add_quotation(char *str)
{
	strcat(strins(str,'\"'),"\"");
	return str;
}

struct td_node {
        int key;
        char colname[31];
        int *pkn;
        char **pks;
};

static int buf_Cmp(void *rec1,void *rec2,int len)
{
register struct td_node *dap1,*dap2;
        dap1=(struct td_node *)rec1;
        dap2=(struct td_node *)rec2;
        if(dap1->key<dap2->key) return -2;
        else if (dap1->key>dap2->key) return 2;
        return 0;
}

static int mkpk(void *content)
{
register struct td_node *pn;
	pn=(struct td_node *)content;
	if(!*pn->pks) {
		*pn->pks=(char *)malloc(strlen(pn->colname)+2);
		sprintf(*pn->pks,"%s|",pn->colname);
	} else {
		*pn->pks=(char *)realloc(*pn->pks,strlen(*pn->pks)+strlen(pn->colname)+2);
		sprintf((*pn->pks)+strlen(*pn->pks),"%s|",pn->colname);
	}
	(*pn->pkn)++;
	return 0;
}

static int par_Cmp(void *rec1,void *rec2,int len)
{
register PATTERN_COL_stu *dap1,*dap2;
int cc;

        dap1=(PATTERN_COL_stu *)rec1;
        dap2=(PATTERN_COL_stu *)rec2;
        cc=strcmp(dap1->COL_NAME,dap2->COL_NAME);
        if(cc<0) return -2;
        else if (cc>0) return 2;
        return 0;
}

//暂存列名
typedef struct {
	char COLUMN_NAME[49];
	T_PkgType *tp;
} colmn_stu;

static int colmn_Cmp(void *rec1,void *rec2,int len)
{
colmn_stu *dap1,*dap2;
int ret;
        dap1=(colmn_stu *)rec1;
        dap2=(colmn_stu *)rec2;
	ret=strcmp(dap1->COLUMN_NAME,dap2->COLUMN_NAME);
        if(ret<0) return -2;
        else if (ret>0) return 2;
        return 0;
}

static int calc_col(PATTERN_stu *pattrec,TAB_COLUMNS_stu *tab_col,char *datetype)
{

	strcpy(pattrec->Fld_Tlb_Name,tab_col->Fld_Tlb_Name);
	strcpy(pattrec->Fld_Column_Name,tab_col->Fld_Column_Name);
	pattrec->Fld_PK=tab_col->Fld_PK;
	*pattrec->Fld_Format=0;

	if(!strncmp(tab_col->Fld_Column_Type,"VARCHAR",7) ||
	   !strcmp(tab_col->Fld_Column_Type,"CHAR")) {
	   	pattrec->Fld_Column_Type=CH_CHAR;
		pattrec->Fld_Column_Len=tab_col->Fld_Column_Len+1;
	} else if(!strcmp(tab_col->Fld_Column_Type,"DATE")) {
	   	pattrec->Fld_Column_Type=CH_DATE;
	   	strcpy(pattrec->Fld_Format,datetype);
		pattrec->Fld_Column_Len=1+strlen(datetype);
	} else if(!strncmp(tab_col->Fld_Column_Type,"TIMESTAMP(6)",12)) {
	   	pattrec->Fld_Column_Type=CH_DATE;
	   	strcpy(pattrec->Fld_Format,YEAR_TO_USEC);
		pattrec->Fld_Column_Len=YEAR_TO_USEC_LEN;
	} else if(!strcmp(tab_col->Fld_Column_Type,"NUMBER")) {
		if(tab_col->Data_Scale>0) { //有小数
			if(tab_col->data_precision>14) {
	   			pattrec->Fld_Column_Type=CH_CNUM;
				pattrec->Fld_Column_Len=tab_col->data_precision+
							tab_col->Data_Scale + 3;
				*pattrec->Fld_Format=0;
			} else {
	   			pattrec->Fld_Column_Type=CH_DOUBLE;
				pattrec->Fld_Column_Len=sizeof(double);
				sprintf(pattrec->Fld_Format,"%%%d.%dlf",
					tab_col->data_precision+2,
					tab_col->Data_Scale);
			}
		} else {
			if(tab_col->data_precision<=0 ||
			   tab_col->data_precision>18) { //大整数
	   			pattrec->Fld_Column_Type=CH_CNUM;
				pattrec->Fld_Column_Len=(tab_col->data_precision>18)?
					tab_col->data_precision+2:40;
			} else {
				switch(tab_col->data_precision) {
				case 1:
				case 2:
	   				pattrec->Fld_Column_Type=CH_TINY;
					pattrec->Fld_Column_Len=1;
					break;
				case 3:
				case 4:
	   				pattrec->Fld_Column_Type=CH_SHORT;
					pattrec->Fld_Column_Len=2;
					break;
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
	   				pattrec->Fld_Column_Type=CH_INT4;
					pattrec->Fld_Column_Len=4;
					break;
				default:
	   				pattrec->Fld_Column_Type=CH_INT64;
					pattrec->Fld_Column_Len=8;
					break;
				}
			}
		}
	} else if(!strcmp(tab_col->Fld_Column_Type,"FLOAT") ||
		  !strcmp(tab_col->Fld_Column_Type,"BINARY_DOUBLE")) {
		pattrec->Fld_Column_Type=CH_DOUBLE;
		pattrec->Fld_Column_Len=sizeof(double);
		if(tab_col->Data_Scale>0) sprintf(pattrec->Fld_Format,"%%%d.%dlf",
			tab_col->data_precision+2,
			tab_col->Data_Scale);
		else strcpy(pattrec->Fld_Format,"%lg");
	} else if(!strcmp(tab_col->Fld_Column_Type,"BINARY_FLOAT")) {
		pattrec->Fld_Column_Type=CH_FLOAT;
		pattrec->Fld_Column_Len=sizeof(float);
		if(tab_col->Data_Scale>0) sprintf(pattrec->Fld_Format,"%%%d.%df",
			tab_col->data_precision+2,
			tab_col->Data_Scale);
		else strcpy(pattrec->Fld_Format,"%g");
	} else if(!strcmp(tab_col->Fld_Column_Type,"RAW")) {
	   	pattrec->Fld_Column_Type=CH_BYTE;
		pattrec->Fld_Column_Len=tab_col->Fld_Column_Len;
	} else if(!strcmp(tab_col->Fld_Column_Type,"LONG")) {
	   	pattrec->Fld_Column_Type=CH_CLOB;
		pattrec->Fld_Column_Len=-1;
	} else {
		ShowLog(1,"%s:invalible type  col_name=%s,col_type=%s",__FUNCTION__,
			tab_col->Fld_Column_Name,tab_col->Fld_Column_Type);
	   	pattrec->Fld_Column_Type=SHORTNULL;
		pattrec->Fld_Column_Len=-1;
		return -1;
	}
	return 0;
}

/* auto make pattern */

int mksrm(SRM *srmp,T_SQL_Connect *SQL_Connect)
{
char stmt[4024];
int ret,n,i,rowid_flg=0;
PATTERN_stu pattrec;
PATTERN_COL_stu upd_patt;
TAB_COLUMNS_stu tab_col;
T_PkgType *tp;
struct td_node pk_node;
T_Tree *root=0,*upd_pa=0,*colmn_tree=0;
DAU patt_DAU,upd_DAU;
int pkn;
colmn_stu col_name;
char *cp;
char *datetype;
INT64 now;

	if(!srmp->tabname || !*srmp->tabname) return -1;
	DAU_init(&patt_DAU,SQL_Connect,0, &tab_col,TAB_COLUMNS_tpl);	//PATTERN_tpl);
	cp=strchr(srmp->tabname,'.');
	if(!cp) {
	    stptok(srmp->tabname,tab_col.Fld_Tlb_Name,sizeof(tab_col.Fld_Tlb_Name)," 	");
	    strupper(tab_col.Fld_Tlb_Name);
	    if(*SQL_Connect->DBOWN)
		stptok(SQL_Connect->DBOWN,tab_col.Fld_Column_Name,sizeof(tab_col.Fld_Column_Name),0);
	    else
		stptok(SQL_Connect->UID,tab_col.Fld_Column_Name,sizeof(tab_col.Fld_Column_Name),0);
	} else { //srmp->tabname="dbown.tablename"
	    stptok(cp+1,tab_col.Fld_Tlb_Name,sizeof(tab_col.Fld_Tlb_Name)," 	");
	    cp=(char *)srmp->tabname;
	    strtcpy(tab_col.Fld_Column_Name,&cp,'.');
	    strupper(tab_col.Fld_Column_Name);
	}
	ShowLog(5,"%s:table name=%s -> %s",__FUNCTION__,srmp->tabname,tab_col.Fld_Tlb_Name);
	now=now_usec();
	n=descDAO(&patt_DAU,stmt);
	if(n<=0) {
		ShowLog(1,"%s:stmt=%s",__FUNCTION__,stmt);
		DAU_free(&patt_DAU);
		return -2;
	}
	srmp->pks=0;
	pk_node.pkn=&pkn;
	pk_node.pks=(char **)&srmp->pks;
	srmp->tp=(T_PkgType *)malloc(sizeof(T_PkgType) * (n+2));
	if(!srmp->tp) {
		ShowLog(1,"tp malloc Error!");
		DAU_free(&patt_DAU);
		return -3;
	}
	tp=srmp->tp;
	tp->offset=-1;
	tp->type=-1;
	cp=getenv("PATTERN_COL");
	if(!cp||!*cp) cp="PATTERN_COL";
	DAU_init(&upd_DAU,SQL_Connect,cp,&upd_patt,PATTERN_COL_tpl);
	strcpy(upd_patt.TAB_NAME,tab_col.Fld_Tlb_Name);
	ret=getUpdDAO(&upd_DAU,stmt);
	if(ret>0) while(!DAU_next(&upd_DAU)) {
		TRIM(upd_patt.COL_NAME);
		TRIM(upd_patt.COL_TYPE);
		if(!strcmp(upd_patt.COL_NAME,"ROWID")) rowid_flg=1;
                upd_pa=BB_Tree_Add(upd_pa,&upd_patt,sizeof(upd_patt),par_Cmp,0);
	}  else ShowLog(5,"%s: ret=%d,%s",__FUNCTION__,ret,stmt);
	DAU_free(&upd_DAU);

	datetype=getenv("NLS_DATE_FORMAT");
	if(!datetype || !*datetype) datetype=(char *)YEAR_TO_SEC;

	for(i=0;!(ret=DAU_next(&patt_DAU));tp++,i++) {
	char pseudo[49];
		stptok(tab_col.Fld_Column_Name,col_name.COLUMN_NAME,sizeof(col_name.COLUMN_NAME),0);
		col_name.tp=NULL;
		*pseudo=0;
		//如果列名含有小写
		if(lcase(tab_col.Fld_Column_Name)){
			strcpy(pseudo,tab_col.Fld_Column_Name);
		}
//calc column
		ret=calc_col(&pattrec,&tab_col,datetype);
// 查找PATTERN_COL表,修正模板
		if(upd_pa) {
		T_Tree *upd;
   			strcpy(upd_patt.COL_NAME,pattrec.Fld_Column_Name);
			upd=BB_Tree_Find(upd_pa,&upd_patt,sizeof(upd_patt),par_Cmp);
			if(upd) {
			PATTERN_COL_stu *upd_p;
				upd_p=(PATTERN_COL_stu *)upd->Content;
				if(!strcmp(upd_p->COL_TYPE,"DELETE")) {//表示删除该列
					i--;
					tp--;
					continue;
				}
				ret=-1;
				if(*upd_p->COL_TYPE)
				ret=mk_sdbc_type(upd_p->COL_TYPE);
				if(ret >= 0) {
					pattrec.Fld_Column_Type=ret;
                        			if(upd_p->COL_LEN>0) {//修正该列的长度
                             			pattrec.Fld_Column_Len=upd_p->COL_LEN;
                        		}
                        		if(*upd_p->COL_FORMAT) strcpy(pattrec.Fld_Format,upd_p->COL_FORMAT);
				}
				if(*pseudo) add_quotation(pattrec.Fld_Column_Name);
				else if(col_to_lower) strlower(pattrec.Fld_Column_Name);
				TRIM(upd_p->PSEUDO_NAME);
				if(*upd_p->PSEUDO_NAME) {
					strcat(pattrec.Fld_Column_Name," ");
					strcat(pattrec.Fld_Column_Name,upd_p->PSEUDO_NAME);
				} else if(*pseudo) {
					strcat(pattrec.Fld_Column_Name," ");
					strcat(pattrec.Fld_Column_Name,pseudo);
				}

			} else THROW catch;
        	} else {
catch:
                    if(*pseudo) {
                        add_quotation(pattrec.Fld_Column_Name);
                        strcat(pattrec.Fld_Column_Name," ");
                        strcat(pattrec.Fld_Column_Name,pseudo);
                    }
                    else if(col_to_lower) strlower(pattrec.Fld_Column_Name);
         	}
		tp->name=strdup(pattrec.Fld_Column_Name);
		tp->type=pattrec.Fld_Column_Type;
		tp->len=pattrec.Fld_Column_Len;
		if(*pattrec.Fld_Format) {
			tp->format=strdup(pattrec.Fld_Format);
		} else tp->format=NULL;
//先记录下来，准备在没有主键时查索引表，确定唯一索引的列
		col_name.tp=tp;
		colmn_tree=BB_Tree_Add(colmn_tree,&col_name,sizeof(col_name),colmn_Cmp,0);
		if(pattrec.Fld_PK > 0) {
			pk_node.key=pattrec.Fld_PK;
			strcpy(pk_node.colname,plain_name(pattrec.Fld_Column_Name));
//ShowLog(5,"%s:PATTERN[%d]:%s,pk=%d",__FUNCTION__,i,pattrec.Fld_Column_Name,pattrec.Fld_PK);
			root=BB_Tree_Add(root,&pk_node,sizeof(pk_node),buf_Cmp,0);
		}
	}
/* 看看是否需要增加一个ROWID列 */
	if(rowid_flg) {
        T_Tree *upd;
		strcpy(upd_patt.COL_NAME,"ROWID");
                upd=BB_Tree_Find(upd_pa,&upd_patt,sizeof(upd_patt),par_Cmp);
                if(upd) {
                PATTERN_COL_stu *upd_p;
                        upd_p=(PATTERN_COL_stu *)upd->Content;
                        if(strcmp(upd_p->COL_TYPE,"DELETE")) {//表示不删除该列
                                if(*upd_p->COL_TYPE)
                                        pattrec.Fld_Column_Type=mk_sdbc_type(upd_p->COL_TYPE);
                                if(upd_p->COL_LEN>0) {//修正该列的长度
                                        pattrec.Fld_Column_Len=upd_p->COL_LEN;
                                }
                                tp->name=strdup("ROWID");
                                tp->type=pattrec.Fld_Column_Type;
                                tp->len=pattrec.Fld_Column_Len;
                                tp->format=0;
                                tp++;
                        }
               }
    }

	DAU_free(&patt_DAU);
	tp->type=-1;
	tp->len=0;
	tp->format=0;
	srmp->result=0;
	srmp->rp=0;
	srmp->Aflg=set_offset(srmp->tp);
	srmp->colidx=mk_col_idx(srmp->tp);
	srmp->rec=(char *)malloc(srmp->tp[srmp->Aflg].offset+1);
	if(!srmp->rec||srmp->tp->type<0) {
		ShowLog(1,"srmp->rec Malloc error!");
		if(root) BB_Tree_Free(&root,0);
		PatternFree(srmp);
		return -4;
	}
	if(!root) do {//没有主键，找一个唯一索引做主键
		ALL_IND_COLUMNS_stu colmn;
		DAU_init(&patt_DAU,SQL_Connect,0,&colmn,ALL_IND_COLUMNS_tpl);
		strcpy(colmn.TABLE_NAME,pattrec.Fld_Tlb_Name);
/*会找到不可识别的索引
		strcpy(stmt,"WHERE TABLE_NAME=:TABLE_NAME AND INDEX_NAME="
			"(SELECT INDEX_NAME FROM ALL_INDEXES "
			"WHERE TABLE_NAME=:TABLE_NAME AND UNIQUENESS='UNIQUE' AND rownum < 2)");
*/
		i=getIdxDao(&patt_DAU,stmt);
		if(i<=0) {
			DAU_free(&patt_DAU);
			break;
		}
		DAU_init(&upd_DAU,SQL_Connect,0,&colmn,ALL_IND_COLUMNS_tpl);
		strcpy(stmt,"WHERE TABLE_NAME=:TABLE_NAME AND INDEX_NAME=:INDEX_NAME");
		upd_DAU.srm.hint="/*+client_result_cache */";
		for(ret=0;ret<i;ret++) {//每个索引名
		int ind_num;
			patt_DAU.srm.rp += net_dispack(colmn.INDEX_NAME,patt_DAU.srm.rp,CharType);
			ind_num=DAU_prepare(&upd_DAU,stmt);
			if(ind_num<0) {
				ShowLog(1,"%s:INDEX_NAME '%s' Not Found err %d,%s",
					__FUNCTION__,colmn.INDEX_NAME,
					SQL_Connect->Errno,
					SQL_Connect->ErrMsg);
				continue;
			}
			while(!DAU_next(&upd_DAU)) {//每个索引列
			T_Tree *upd;
			T_PkgType *tpe;
				strcpy(col_name.COLUMN_NAME,colmn.COLUMN_NAME);
       	         		upd=BB_Tree_Find(colmn_tree,&col_name,sizeof(col_name),colmn_Cmp);
				if(!upd) { //不可识别的索引列
				   strcpy(upd_patt.COL_NAME,colmn.COLUMN_NAME);
				   upd=BB_Tree_Find(upd_pa,&upd_patt,sizeof(upd_patt),par_Cmp);
				   if(!upd) {
bad_colname:
					ShowLog(1,"UK:索引名 %s 键列序：%d 可将 %s|%s|PK|||对应的真实列名| "
						  "加入 PATTERN_COL 表",
						colmn.INDEX_NAME,colmn.COLUMN_POSITION,
						colmn.TABLE_NAME,colmn.COLUMN_NAME);
					BB_Tree_Free(&root,0);
					break;
				   }
				   strcpy(col_name.COLUMN_NAME,((PATTERN_COL_stu *)upd->Content)->PSEUDO_NAME);
       	         		   upd=BB_Tree_Find(colmn_tree,&col_name,sizeof(col_name),colmn_Cmp);
				   if(!upd) THROW bad_colname;
				}
				tpe=((colmn_stu *)upd->Content)->tp;
				pk_node.key=colmn.COLUMN_POSITION;
				strcpy(pk_node.colname,plain_name(tpe->name));
				root=BB_Tree_Add(root,&pk_node,sizeof(pk_node),buf_Cmp,0);
			}
			if(root) break;
			//这个索引名不行了，找下一个
		}
		DAU_free(&patt_DAU);
		DAU_free(&upd_DAU);
	} while(0);
	if(upd_pa) BB_Tree_Free(&upd_pa,0);
	if(colmn_tree)  BB_Tree_Free(&colmn_tree,0);
	if(root) {
		BB_Tree_Scan(root,mkpk);
		BB_Tree_Free(&root,0);
		tp->format=srmp->pks;
		ShowLog(5,"mkpk:%s,TIMES=%d",srmp->pks?srmp->pks:"",(int)(now_usec() - now));
	}

	tp->name=srmp->tabname;
	return 0;
}
