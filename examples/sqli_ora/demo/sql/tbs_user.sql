create user TBSDBA
  identified by tbsdba123
  default tablespace ticket;


GRANT CREATE SESSION                    TO TBSDBA;
GRANT ALTER SESSION                     TO TBSDBA;
GRANT DBA	                          TO TBSDBA;

