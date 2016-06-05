create or replace function fun_compute_zx_fee
(start_station varchar2,end_station varchar2,trans_type varchar2,container_type varchar2)
return number is
  zx_fee number(10,2);
  
  /*
         创建日期：2007-02-15
         创建人：北京交大思源科技有限公司-康日红
         描述：用于计算两个站之间某一种运输方式的整箱费用（一箱价）
  */
begin
  select nvl(tb1.fld_fee_rate,0)+nvl(tb2.fld_fee_rate,0)+nvl(tb3.fld_fee_rate,0) into zx_fee
  from 
  (
      select fld_price as fld_fee_rate,'1' as row_id
      from zd_static_price
      where 
      fld_container_type=container_type
      and fld_start_station=start_station and fld_end_station=end_station
  )tb1 left join
  (
      select 
      sum(zd_show_price.fld_fee_rate) as fld_fee_rate,'1' as row_id
      from zd_fee,zd_show_price,zd_org_station
      where 
      fld_fee_owner='1' and trans_type like fld_get_condition and substr(fld_fee_type,3,1) = '1'
      and zd_show_price.fld_org_code=zd_org_station.fld_org_code
      and zd_show_price.fld_container_type=container_type
      and zd_fee.fld_fee_code=zd_show_price.fld_fee_code
      and zd_org_station.fld_station_code=start_station
      group by zd_org_station.fld_station_code
  )tb2 on tb1.row_id=tb2.row_id
  left join
  (
      select 
      sum(zd_show_price.fld_fee_rate) as fld_fee_rate,'1' as row_id
      from zd_fee,zd_show_price,zd_org_station
      where 
      fld_fee_owner='3' and trans_type like fld_get_condition and substr(fld_fee_type,3,1) = '1'
      and zd_show_price.fld_org_code=zd_org_station.fld_org_code
      and zd_show_price.fld_container_type=container_type
      and zd_fee.fld_fee_code=zd_show_price.fld_fee_code
      and zd_org_station.fld_station_code=end_station
      group by zd_org_station.fld_station_code
  )tb3 on tb1.row_id=tb3.row_id;
  
  return zx_fee;

end;

  

