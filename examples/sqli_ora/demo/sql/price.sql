drop table TRAIN_PRICE;
create table TRAIN_PRICE (     /* 车次表 */
    id	 	   number(6),
    start_station char(3), /* 始发站  */
    end_station char(3),   /* 终到站 */
    beg_date    date,       /* 有效起始日 YYYY-MM-DD */
    end_date    date,       /* 有效截止日 YYYY-MM-DD */
    yz_fee     number(7),       /* 硬座费 全部费以角为单位 */
    rz_fee     number(7),       /* 软座费 */
	rz1_fee		number(7),       /* 一等软座费 */
	rz2_fee		number(7),       /* 二等软座费 */
    yws_fee    number(7),       /* 硬卧上 */
    ywz_fee    number(7),       /* 硬卧中 */
    ywx_fee    number(7),       /* 硬卧下 */
    rws_fee    number(7),       /* 软卧上 */
    rwx_fee    number(7),       /* 软卧下 */
    g2s_fee    number(7),       /* 二人高包上 */
    g2x_fee    number(7),       /* 二人高包下 */
    note        varchar2(50) null,      /* 备注  */
    primary key (id,start_station,end_station,end_date)
);

 -- select * from TRAIN_PRICE where id=30301 AND start_station='VNP' and end_station='TJP' and end_date >= to_date('2009-0731','YYYY-MM-DD');
