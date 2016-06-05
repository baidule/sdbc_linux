/*
create table STUDENT
(
  STUID     NUMBER,
  STUNAME   VARCHAR2(10),
  STUGENDER CHAR(1),
  STUCLASS  VARCHAR2(10)
);


insert into student (STUID, STUNAME, STUGENDER, STUCLASS)
values (3, 'ly', '1', '0');

insert into student (STUID, STUNAME, STUGENDER, STUCLASS)
values (4, 'lq', '0', '0');

insert into student (STUID, STUNAME, STUGENDER, STUCLASS)
values (1, 'sxs', '1', '0');

insert into student (STUID, STUNAME, STUGENDER, STUCLASS)
values (2, 'jxf', '0', '0');
commit;

create or replace package test_buspage is
  type myCursor is ref cursor;
*/
CREATE OR REPLACE PACKAGE Hstype IS
   
  TYPE t_cursor IS REF CURSOR; 
 
end Hstype; 
/

CREATE OR REPLACE function TESTCURSOR (p_input   IN     VARCHAR2,
                                    out_cursor   out Hstype.t_cursor,
                                    out_total    out string,
                                    out_errno out string,
                                    out_messages out string
                    )
   return number
     as
 v_sql    varchar2(4000);

   BEGIN
   out_errno:=555;
   out_messages :='³É¹¦';
   out_total :=0;
   
   select count(1) into out_total from student ;
      
     v_sql :='   select a.* 
     from  student a  ';
    open out_cursor for v_sql;

  return 0;
   END TESTCURSOR;
/ 
   
   
   
