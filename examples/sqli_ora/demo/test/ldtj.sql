DECLARE
   lv_f utl_file.file_type;
   lv_data VARCHAR2(1000);
   TYPE t_str IS TABLE OF VARCHAR2(1000) INDEX BY BINARY_INTEGER ;
   lv_cnt NUMBER :=0;
   lv_str t_str;
   n NUMBER := 0;
BEGIN

--   create or replace directory TJRB as '/home/tuxticket/test';

   lv_f := UTL_FILE.FOPEN ('TJRB','TJ.txt','r');
   LOOP
      BEGIN
         UTL_FILE.GET_LINE (lv_f,lv_data,1000);
      EXCEPTION
         WHEN NO_DATA_FOUND THEN
              EXIT;
      END;
      lv_cnt := lv_cnt+1;
      lv_str(lv_cnt):=lv_data;
   END LOOP;
   UTL_FILE.FCLOSE(lv_f);
   
   FORALL i IN 1..lv_cnt
	 n := i;
         INSERT INTO TJRB VALUES (
                TO_DATE(SUBSTR(lv_str(i),1,INSTR(lv_str(i),'|',1)-1),'YYYY.MM.DD')
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1, 1)+1,INSTR(lv_str(i),'|',1, 2) - INSTR(lv_str(i),'|',1, 1) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1, 2)+1,INSTR(lv_str(i),'|',1, 3) - INSTR(lv_str(i),'|',1, 2) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1, 3)+1,INSTR(lv_str(i),'|',1, 4) - INSTR(lv_str(i),'|',1, 3) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1, 4)+1,INSTR(lv_str(i),'|',1, 5) - INSTR(lv_str(i),'|',1, 4) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1, 5)+1,INSTR(lv_str(i),'|',1, 6) - INSTR(lv_str(i),'|',1, 5) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1, 6)+1,INSTR(lv_str(i),'|',1, 7) - INSTR(lv_str(i),'|',1, 6) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1, 7)+1,INSTR(lv_str(i),'|',1, 8) - INSTR(lv_str(i),'|',1, 7) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1, 8)+1,INSTR(lv_str(i),'|',1, 9) - INSTR(lv_str(i),'|',1, 8) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1, 9)+1,INSTR(lv_str(i),'|',1,10) - INSTR(lv_str(i),'|',1, 9) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,10)+1,INSTR(lv_str(i),'|',1,11) - INSTR(lv_str(i),'|',1,10) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,11)+1,INSTR(lv_str(i),'|',1,12) - INSTR(lv_str(i),'|',1,11) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,12)+1,INSTR(lv_str(i),'|',1,13) - INSTR(lv_str(i),'|',1,12) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,13)+1,INSTR(lv_str(i),'|',1,14) - INSTR(lv_str(i),'|',1,13) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,14)+1,INSTR(lv_str(i),'|',1,15) - INSTR(lv_str(i),'|',1,14) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,15)+1,INSTR(lv_str(i),'|',1,16) - INSTR(lv_str(i),'|',1,15) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,16)+1,INSTR(lv_str(i),'|',1,17) - INSTR(lv_str(i),'|',1,16) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,17)+1,INSTR(lv_str(i),'|',1,18) - INSTR(lv_str(i),'|',1,17) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,18)+1,INSTR(lv_str(i),'|',1,19) - INSTR(lv_str(i),'|',1,18) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,19)+1,INSTR(lv_str(i),'|',1,20) - INSTR(lv_str(i),'|',1,19) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,20)+1,INSTR(lv_str(i),'|',1,21) - INSTR(lv_str(i),'|',1,20) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,21)+1,INSTR(lv_str(i),'|',1,22) - INSTR(lv_str(i),'|',1,21) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,22)+1,INSTR(lv_str(i),'|',1,23) - INSTR(lv_str(i),'|',1,22) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,23)+1,INSTR(lv_str(i),'|',1,24) - INSTR(lv_str(i),'|',1,23) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,24)+1,INSTR(lv_str(i),'|',1,25) - INSTR(lv_str(i),'|',1,24) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,25)+1,INSTR(lv_str(i),'|',1,26) - INSTR(lv_str(i),'|',1,25) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,26)+1,INSTR(lv_str(i),'|',1,27) - INSTR(lv_str(i),'|',1,26) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,27)+1,INSTR(lv_str(i),'|',1,28) - INSTR(lv_str(i),'|',1,27) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,28)+1,INSTR(lv_str(i),'|',1,29) - INSTR(lv_str(i),'|',1,28) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,29)+1,INSTR(lv_str(i),'|',1,30) - INSTR(lv_str(i),'|',1,29) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,30)+1,INSTR(lv_str(i),'|',1,31) - INSTR(lv_str(i),'|',1,30) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,31)+1,INSTR(lv_str(i),'|',1,32) - INSTR(lv_str(i),'|',1,31) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,32)+1,INSTR(lv_str(i),'|',1,33) - INSTR(lv_str(i),'|',1,32) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,33)+1,INSTR(lv_str(i),'|',1,34) - INSTR(lv_str(i),'|',1,33) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,34)+1,INSTR(lv_str(i),'|',1,35) - INSTR(lv_str(i),'|',1,34) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,35)+1,INSTR(lv_str(i),'|',1,36) - INSTR(lv_str(i),'|',1,35) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,36)+1,INSTR(lv_str(i),'|',1,37) - INSTR(lv_str(i),'|',1,36) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,37)+1,INSTR(lv_str(i),'|',1,38) - INSTR(lv_str(i),'|',1,37) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,38)+1,INSTR(lv_str(i),'|',1,39) - INSTR(lv_str(i),'|',1,38) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,39)+1,INSTR(lv_str(i),'|',1,40) - INSTR(lv_str(i),'|',1,39) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,40)+1,INSTR(lv_str(i),'|',1,41) - INSTR(lv_str(i),'|',1,40) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,41)+1,INSTR(lv_str(i),'|',1,42) - INSTR(lv_str(i),'|',1,41) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,42)+1,INSTR(lv_str(i),'|',1,43) - INSTR(lv_str(i),'|',1,42) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,43)+1,INSTR(lv_str(i),'|',1,44) - INSTR(lv_str(i),'|',1,43) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,44)+1,INSTR(lv_str(i),'|',1,45) - INSTR(lv_str(i),'|',1,44) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,45)+1,INSTR(lv_str(i),'|',1,46) - INSTR(lv_str(i),'|',1,45) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,46)+1,INSTR(lv_str(i),'|',1,47) - INSTR(lv_str(i),'|',1,46) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,47)+1,INSTR(lv_str(i),'|',1,48) - INSTR(lv_str(i),'|',1,47) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,48)+1,INSTR(lv_str(i),'|',1,49) - INSTR(lv_str(i),'|',1,48) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,49)+1,INSTR(lv_str(i),'|',1,50) - INSTR(lv_str(i),'|',1,49) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,50)+1,INSTR(lv_str(i),'|',1,51) - INSTR(lv_str(i),'|',1,50) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,51)+1,INSTR(lv_str(i),'|',1,52) - INSTR(lv_str(i),'|',1,51) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,52)+1,INSTR(lv_str(i),'|',1,53) - INSTR(lv_str(i),'|',1,52) -1)
               ,SUBSTR(lv_str(i),INSTR(lv_str(i),'|',1,53)+1,INSTR(lv_str(i),'|',1,54) - INSTR(lv_str(i),'|',1,53) -1)
        );                                      
        EXCEPTION
		WHEN DUP_VAL_ON_INDEX THEN
			dbms_output.put_line('no=4的记录找不到.直接返回,no=4的就不找了，呵呵');


		WHEN OTHERS THEN
			dbms_output.put_line('bad_credit');
			NULL;

END;
/

