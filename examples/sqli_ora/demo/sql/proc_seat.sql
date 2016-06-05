CREATE OR REPLACE TYPE SEAT AS TABLE OF tp_seat2
/


CREATE OR REPLACE PROCEDURE getseat_for_sell (
                            p_result      OUT SYS_REFCURSOR     ---- 结果集
                           ,p_count       OUT NUMBER            ---- 获得的票数。假如没有足够的票, 可能<p_quantity
                           ,p_start_date  IN  DATE
                           ,p_train_no    IN  VARCHAR2
                           ,p_beg_station IN  VARCHAR2
                           ,p_arrive      IN  NUMBER
                           ,p_xb          IN  NUMBER
                           ,p_purpose     IN  NUMBER
                           ,p_quantity    IN  NUMBER
                           ,p_userid      IN  VARCHAR2
                           ,p_devid       IN  VARCHAR2
                            )
AS
   lv_flag       NUMBER;
   e_locked      EXCEPTION;
   PRAGMA EXCEPTION_INIT (e_locked, -54);
   lv_result tp_seat_table2 := tp_seat_table2();
   
BEGIN
   p_count := 0;
   /* 先收集ROWID */
   FOR lv_rec IN (SELECT rowid AS row_id,seat.*
                    FROM seat
                   WHERE start_date        = p_start_date
                         AND beg_station   = p_beg_station
                         AND train_no      = p_train_no
                         AND (MOD(p_xb,10) = 0 AND seat_type > p_xb AND seat_type <= p_xb+9
                              OR MOD(p_xb,10) <> 0 AND seat_type = p_xb
                              )
                         AND end_station  >= p_arrive
                         AND purpose      =  p_purpose
                         AND flag         =  0
                   ORDER BY end_station,carno,seat_no,seat_type*DECODE(p_xb,0,-1,1)
                 )
   LOOP
       lv_flag := NULL;
/* 找到ROWID中还没有被抢走的*/
       BEGIN
          SELECT flag
            INTO lv_flag
            FROM seat
           WHERE rowid = lv_rec.row_id
           FOR UPDATE NOWAIT;
       EXCEPTION
          WHEN e_locked THEN
               lv_flag := NULL;
       END;
/* 占用 */
       IF lv_flag = 0 THEN
          UPDATE seat
             SET flag      =1
                ,used_time = SYSDATE
                ,used_uid  = p_userid
                ,used_dev  = p_devid
           WHERE rowid = lv_rec.row_id;
/* 打包输出 */
          p_count := p_count+1;
          lv_result.EXTEND;
          lv_result(p_count) := tp_seat2(TO_CHAR(lv_rec.start_date,'YYYY-MM-DD')
                                        ,lv_rec.beg_station
                                        ,lv_rec.train_no
                                        ,lv_rec.run_train
                                        ,TO_CHAR(lv_rec.on_date,'YYYY-MM-DD HH24:MI')
                                        ,lv_rec.carno
                                        ,lv_rec.seat_type
                                        ,lv_rec.seat_no
                                        ,lv_rec.end_station
                                        ,lv_rec.shortest_station
                                        ,lv_rec.purpose
                                        ,lv_rec.gride
                                        ,lv_rec.pro
                                        ,lv_rec.flag
                                        ,lv_rec.used_dev
                                        ,lv_rec.used_uid
                                        ,TO_CHAR(lv_rec.used_time,'YYYY-MM-DD HH24:MI:SS') 
                                        ,ROWIDTOCHAR(lv_rec.row_id)
                                       );
          
          IF p_count >= p_quantity THEN
             EXIT;
          END IF;
       END IF;
   END LOOP;
   
   OPEN p_result FOR
   SELECT *                
     FROM TABLE(CAST(lv_result AS tp_seat_table2)) r
   ORDER BY end_station,carno,seat_no,seat_type*DECODE(p_xb,0,-1,1);
   
END getseat_for_sell;
/
/*

"getseat_for_sell(:cursor,:num,seat.start_date, seat.train_no ,seat.beg_station,seat.end_statio ,seat.seat_type ,
		seat.purpose ,quantity ,userid ,devid)"
*/

