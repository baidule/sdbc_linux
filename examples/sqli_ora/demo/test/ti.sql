create or replace procedure ti
IS
   I   integer;
BEGIN
   FOR I IN 1..100000 LOOP
       INSERT INTO T VALUES (I,I,I,I);
   END LOOP;
END;
/

GRANT EXECUTE on ti TO dict,oper;
