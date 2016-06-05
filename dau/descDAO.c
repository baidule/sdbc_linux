/********************************************************************
 * 自动生成模板的程序
 * 根据数据库表结构生成模板。需要一个辅助的表：PATTERN_COL,
 * 对数据结构进行补充定义,这个表可以不存在或空，表示没有补充和修正
 * 如果对某表的某列有补充和修正，就在这个表中建立一条记录。
 * 本程序适用于ORACLE
#include <DAU.h>
#include "gettab.h"
NLS_DATE_FORMAT
 ********************************************************************/

static T_PkgType PATTERN_COL_tpl[]={
        {CH_CHAR,49,"TAB_NAME",0,-1},
        {CH_CHAR,49,"COL_NAME",0},
        {CH_CHAR,13,"COL_TYPE",0},
        {CH_INT,4,"COL_LEN",0},
        {CH_CHAR,37,"COL_FORMAT",0},
	{CH_CHAR,25,"PSEUDO_NAME"},
        {-1,0,0,0}
};
/* Formatted on 2011/09/29 */
static T_PkgType TAB_COLUMNS_tpl[]={
	{CH_CHAR,49,"c.TABLE_NAME Fld_Tlb_Name",0,-1},
	{CH_CHAR,49,"c.COLUMN_NAME  Fld_Column_Name"},
	{CH_CHAR,31,"c.DATA_TYPE Fld_Column_Type"},
	{CH_SHORT,sizeof(short),"DATA_LENGTH Fld_Column_Len"},
	{CH_SHORT,sizeof(short),"DATA_PRECISION"},
	{CH_SHORT,sizeof(short),"c.DATA_SCALE Data_Scale"},
	{CH_SHORT,sizeof(short),"k.POSITION Fld_PK"},
	{-1,0,"ALL_TAB_COLUMNS c, "				//表名表达式 
	      "(SELECT C2.TABLE_NAME,C2.COLUMN_NAME,C2.POSITION "
	      "FROM USER_CONSTRAINTS C1,USER_CONS_COLUMNS C2 "
	      "WHERE C1.OWNER = :Fld_Column_Name AND "
	      "C1.TABLE_NAME=:Fld_Tlb_Name AND C1.CONSTRAINT_TYPE='P' AND "
	      "C2.CONSTRAINT_NAME=C1.CONSTRAINT_NAME) k ",0}
};

/* auto make pattern */

static int descDAO(DAU *DP,char *stmt)
{
int ret;

// 如果是其它数据库，要改。
	strcpy(stmt,"WHERE c.TABLE_NAME = k.TABLE_NAME(+) "
                 "AND c.COLUMN_NAME = k.COLUMN_NAME(+) "
                 "AND c.OWNER = :Fld_Column_Name AND c.TABLE_NAME=:Fld_Tlb_Name "
                 "ORDER BY c.TABLE_NAME, c.COLUMN_ID ");
//	DP->srm.hint="/*+client_result_cache*/";
	ret=DAU_select(DP,stmt,0);
	if(ret<=0) ShowLog(1,"descDAO:DAU_select err=%d,%s",
		DP->SQL_Connect->Errno,DP->SQL_Connect->ErrMsg);
	return ret;
}

static int getUpdDAO(DAU *DP,char *stmt)
{
	mk_where("TAB_NAME",stmt);
//	DP->srm.hint="/*+client_result_cache*/";
        return DAU_select(DP,stmt,0);
}

static int getIdxDao(DAU *DP,char *stmt)
{
	strcpy(stmt,"SELECT INDEX_NAME FROM ALL_INDEXES "
              "WHERE TABLE_NAME=:TABLE_NAME AND UNIQUENESS='UNIQUE'");
	return DAU_select(DP,stmt,0);
}

