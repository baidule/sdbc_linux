create user dict
  identified by kpdict234
  default tablespace ticket;

create user oper
  identified by kpoper567
  default tablespace ticket;

create user query
  identified by kpquery890
  default tablespace ticket;

GRANT CREATE SESSION                    TO dict;
GRANT ALTER SESSION                     TO dict;
GRANT RESOURCE                          TO dict;

GRANT CREATE SESSION                    TO oper;
GRANT ALTER SESSION                     TO oper;
GRANT RESOURCE                          TO oper;

GRANT CREATE SESSION                    TO query;
GRANT ALTER SESSION                     TO query;
GRANT CONNECT                           TO query;
