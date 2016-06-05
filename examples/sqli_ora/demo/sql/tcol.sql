
create or replace procedure createtab(colc IN NUMBER,PP IN NUMBER:=15,SS IN NUMBER:=2)
AUTHID CURRENT_USER

    is
    buf varchar(24000):='create table tcol'||to_char(colc)||'(x1 varchar(10),';
    begin
    for i in 1..colc-1
    loop
    buf:=buf||'v'||to_char(i)||' number('||to_char(PP)||','||to_char(SS)||'),';
    end loop;
  buf:=buf||'v'||to_char(colc)||' number('||to_char(PP)||','||to_char(SS)||'))';
  execute immediate buf;
  end;
/
SHOW ERROR PROCEDURE createtab;

