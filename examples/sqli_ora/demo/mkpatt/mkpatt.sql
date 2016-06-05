/* 模板生成器用，列修改信息 */
drop table PATTERN_COL ;
create table PATTERN_COL (
        TAB_NAME                VARCHAR2(48), -- 表名
        COL_NAME                VARCHAR2(48), -- 列名，可附加ROWID列 
        COL_TYPE                VARCHAR2(12), -- 列类型，为SDBC类型，DELETE表示该列不生成模板。
        COL_LEN                 NUMBER(5),	  -- 列长度，按SDBC模板要求
        COL_FORMAT              VARCHAR2(36), -- 列格式
        PSEUDO_NAME             varchar2(24), -- 伪列名
        primary key (TAB_NAME,COL_NAME)
);

