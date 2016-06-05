/********************************************************************
 * 自动生成模板的程序
 * 根据数据库表结构生成模板。需要一个辅助的表：PATTERN_COL,
 * 对数据结构进行补充定义,这个表可以不存在或空，表示没有补充和修正
 * 如果对某表的某列有补充和修正，就在这个表中建立一条记录。
 ********************************************************************/
int mksrm(SRM *srmp,T_SQL_Connect *SQL_Connect);

//extern T_PkgType PATTERN_COL_tpl[];
typedef struct {
        char TAB_NAME[49];
        char COL_NAME[49];
        char COL_TYPE[13];
        int COL_LEN;
        char COL_FORMAT[37];
	char  PSEUDO_NAME[25];
} PATTERN_COL_stu;

//extern T_PkgType PATTERN_tpl[];
typedef struct  {
	char Fld_Tlb_Name[49];
	char Fld_Column_Name[83];
	short Fld_Column_Type;
	short Fld_Column_Len;
	char Fld_Format[30];
	short Fld_PK;
} PATTERN_stu;

typedef struct {
	char Fld_Tlb_Name[49];
	char Fld_Column_Name[49];
	char Fld_Column_Type[31];
	short Fld_Column_Len;
	short data_precision;
	short Data_Scale;
	short Fld_PK;
} TAB_COLUMNS_stu;

