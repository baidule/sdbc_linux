/********************************************************************
 * �Զ�����ģ��ĳ���
 * �������ݿ��ṹ����ģ�塣��Ҫһ�������ı�PATTERN_COL,
 * �����ݽṹ���в��䶨��,�������Բ����ڻ�գ���ʾû�в��������
 * �����ĳ���ĳ���в��������������������н���һ����¼��
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

