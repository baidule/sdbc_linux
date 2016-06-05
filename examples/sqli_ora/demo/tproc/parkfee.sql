CREATE OR REPLACE PROCEDURE CMS_PROD.TESTFEE (p_input   IN     VARCHAR2,
                   p_out        OUT VARCHAR2,
                   p_out2       OUT VARCHAR2)
   IS
   BEGIN
      p_out := 'ok1';
      p_out2 := 'ok2';
   END;
