
ora <<!
-- 清除共享语句池 
alter system flush shared_pool;
-- 清除数据缓冲区 
alter session set events 'immediate trace name flush_cache level 1';
!

