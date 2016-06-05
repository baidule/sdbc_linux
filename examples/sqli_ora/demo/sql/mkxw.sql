begin
/*
 make_seats (
          p_train_no     IN VARCHAR2　DEFAULT NULL
         ,p_dflg         IN NUMBER    DEFAULT 0    ---- 如果1则不用TRAINS里面的偏移量
         ,p_offset_days  IN NUMBER    DEFAULT 0    ---- 缺省当天则偏移为0, +1为第二天
         ,p_created_days IN NUMBER    DEFAULT 1    ---- 缺省一天
         )
*/

	make_seats(null,0,0,1);
end;
/
