/****************************************************
 * 席位发布 新的母表格式
 * usage sample:
 * xwfb -f xbfb.ini -d 日期表达式 -c 车次 -n 车厢号,车厢号,...
 ****************************************************/
#include <kpapp.h>
#include <libgen.h>

static INT4 today;
static int dflg=0;

static int xwfb(DAU *train_DAU,char *carno);
// 以下定义DAO函数
static int adj_remant(DAU *DP,char *stmt)
{
int ret;
char tmp[2048],*p;
int amoung;

	p=stmt;
	amoung=((SEAT_REMANT_stu *)DP->srm.rec)->amoung;
	if(DP->cursor<0) {
	char *p;
		p=DAU_mk_where(DP,DP->srm.pks,stmt);
		p+=sprintf(p," for update");
	}
	if((ret=DAU_prepare(DP,stmt)) || DAU_next(DP)) {
		if(DP->SQL_Connect->Errno == SQLNOTFOUND)
			return DAU_insert(DP,stmt);
		ShowLog(1,"adj_remant stmt=%s",stmt);
		return ret;
	}
	((SEAT_REMANT_stu *)DP->srm.rec)->amoung += amoung ;
	ret=update_by_PK(DP,stmt);
	if(ret != 1) {
		ShowLog(1,"adj_remant stmt= %s",stmt);
	}
	return 0;
}

int addxw(DAU *DP,char *stmt)
{
int ret;
	if(ret=DAU_insert(DP,stmt)) {
		if(DP->SQL_Connect->Errno == DUPKEY) { //dummy_update
			dummy_update(DP,stmt);
		} else  ShowLog(1,"addxw:%s",stmt);
	}
	return ret;
}
/* 确定了车次，没有确定车站，可能会有若干车站 */
inline int stop_station_dao(DAU *DP,char *stmt)
{
int ret;

	if(DP->cursor<0)
 		sprintf(stmt,"WHERE train_no=:train_no AND "
			"beg_date <= :beg_date AND end_date >= :beg_date AND flag=0 AND over>0 ");
	else *stmt=0;
	ret = DAU_select(DP,stmt,0);
//	ShowLog(5,"stop_station_dao:ret=%d,stmt=%s",ret,stmt);
	return ret;
}
// 车次车站都确定了，只有一条记录
inline int get_stop_station(DAU *DP,char *stmt)
{
char *p;

	if(DP->cursor<0) {
		p=DAU_mk_where(DP,"train_no,sequence",stmt);
		sprintf(p," AND beg_date <= :beg_date AND end_date >= :beg_date AND flag=0");
	}
	if(0 > DAU_prepare(DP,stmt)) {
		ShowLog(1,"get_stop_station stmt=%s",stmt);
		return DP->SQL_Connect->Errno;
	}
	return DAU_next(DP);
}

inline int train_dao(DAU *train_DAU,char *stmt)
{
TRAIN_stu *train;
char *p;

	train=(TRAIN_stu *)train_DAU->srm.rec;
	p=stmt;
	p+=sprintf(p,"WHERE flag=0 AND end_date >= :beg_date ");		//today存放在beg_date
	if(*train->train_no) 
		p+=sprintf(p,"AND train_no=:train_no");
	return DAU_select(train_DAU,stmt,0);
}

INT4 getjobday(DAU *DP,char *stmt)
{
/* 应该根据权限查找相应的天数 */
	return 10;
}
//DAO函数定义完毕

static int mkxw(DAU *pre_DAU,char *stmt,char *day,int start_time, char *carno)
{
int ret,i;
INT64 now;
char s_now[30];
INT4 jobday;
int totalnum=0;
PRE_SEAT_INDEX_stu *pre_index;
PRE_SEAT_stu pre_seat;
SEAT_stu seat;
SEAT_REMANT_stu remant;
STOP_STATION_stu stop_station;
DAU pre_seat_DAU,cz_DAU,seat_DAU,remant_DAU;

	DAU_init(&pre_seat_DAU,pre_DAU->SQL_Connect,0,&pre_seat,PRE_SEAT_tpl);
	DAU_init(&cz_DAU,pre_DAU->SQL_Connect,0,&stop_station,STOP_STATION_tpl);
	DAU_init(&seat_DAU,pre_DAU->SQL_Connect,0,&seat,SEAT_tpl);
	DAU_init(&remant_DAU,pre_DAU->SQL_Connect,0,&remant,SEAT_REMANT_tpl);

	pre_index=(PRE_SEAT_INDEX_stu *)pre_DAU->srm.rec;
	jobday=rstrfmttojul(day,YEAR_TO_DAY);
	strcpy(stop_station.train_no,pre_index->train_no);

	rjultostrfmt(s_now,pre_index->beg_date,"YYYYMMDD");
	sprintf(pre_seat.pre_id,"%s:%c:%s", pre_index->train_no,*pre_index->item,s_now);
	DAU_mk_where(&pre_seat_DAU,"pre_id",stmt);
	if(0 >= (ret=DAU_select(&pre_seat_DAU,stmt,0))) {
		ShowLog(1,"mkxw stmt=%s,err=%d.%s",stmt);
		return ret;
	}

	now=now_sec();
	strcpy(seat.start_date,day);
	strcpy(seat.used_dev,"xwfb");
	strcpy(seat.used_uid,seat.used_dev);
	seat.used_time=now;
	seat.flag=0; // 应该是＝3，暂不可用
	strcpy(seat.train_no,pre_index->train_no);
	pre_index->distribute_time=now;

	while(!DAU_next(&pre_seat_DAU)) {
		stop_station.sequence=pre_seat.beg_station;
		stop_station.beg_date=jobday;
		if(get_stop_station(&cz_DAU,stmt)) continue;

		i=start_time+stop_station.running_time+stop_station.stop_time;
		ret=jobday*1440+timezone/60+i;
		rminstrfmt(seat.on_date,ret,YEAR_TO_MIN);		//上车时间
		strcpy(seat.beg_station,stop_station.station_code);	//上车站代码
		strcpy(seat.run_train,stop_station.run_train_no);
		seat.carno=pre_seat.carno;
		seat.end_station=pre_seat.end_station;
		seat.shortest_station=pre_seat.shortest_station;
		seat.purpose=pre_seat.purpose;
		strcpy(seat.gride,pre_seat.gride);
		DAU_copy(&remant_DAU,&seat_DAU,0);
		remant.seat_type = pre_seat.seat_type;
		remant.amoung=0;

		for(i=pre_seat.beg_seatno;i<=pre_seat.end_seatno;i++) {
			seat.seat_no=i;
			*seat.pro=0; //应该查编组库确定席位属性
			seat.seat_type=pre_seat.seat_type;
			if(pre_seat.seat_type==20) { //硬卧
			int j;
				for(j=0;j<3;j++) {
					seat.seat_type++;
					ret=addxw(&seat_DAU,stmt);
					if(!ret) remant.amoung++,totalnum++;
				}
			} else {
				if(pre_seat.seat_type==30) { //软卧
					seat.seat_type++;
					if(!(i&1)) seat.seat_type++;
				}
				ret=addxw(&seat_DAU,stmt);
				if(!ret) remant.amoung++,totalnum++;
			}

		}
ShowLog(2,"mkxw train=%s:%s,beg_station=%s,end_station=%d,carno=%d,remant=%d",
remant.on_date,remant.run_train,remant.beg_station,remant.end_station,remant.carno,remant.amoung);

		if(!seat.flag && remant.amoung>0)
			ret=adj_remant(&remant_DAU,stmt);

		if(pre_DAU->upd_sth<0) {
		char *p=stmt;

			p=DAU_mk_update(pre_DAU,p);
			p+=sprintf(p," SET distribute_time=:distribute_time ");
			p=DAU_mk_where(pre_DAU,pre_DAU->srm.pks,p);
		}
		if(1 != DAU_update(pre_DAU,stmt)) {
			ShowLog(1,"mkxw stmt= %s",stmt);
		}
	}
	DAU_free(&pre_seat_DAU);
	DAU_free(&cz_DAU);
	DAU_free(&seat_DAU);
	DAU_free(&remant_DAU);
	return totalnum;
}

int xwfb(DAU  *train_DAU,char *carno)
{
char *p,stmt[4096],tmp[4096],day[30];
int ret,i,i_item,tnum;
INT4 jobday,totalnum=0;
INT64 now;
TRAIN_stu *train;
SEAT_stu seat;
PRE_SEAT_INDEX_stu pre_rec;
SEAT_REMANT_stu remant;
STOP_STATION_stu stop_station;
DAU stop_station_DAU,seat_DAU,seat_remant_DAU,pre_seat_DAU;

	train=(TRAIN_stu *)train_DAU->srm.rec;
	now=now_sec();
	rjultostrfmt(day,today,YEAR_TO_DAY);
	trans_begin(train_DAU->SQL_Connect);
	ret=train_dao(train_DAU,stmt);
	if(ret<=0) {
		ShowLog(1,"没找到车次 %s",stmt);
		return -1;
	}
	DAU_init(&pre_seat_DAU,train_DAU->SQL_Connect,0,&pre_rec,PRE_SEAT_INDEX_tpl);
	DAU_init(&stop_station_DAU,train_DAU->SQL_Connect,0,&stop_station,STOP_STATION_tpl);
	DAU_init(&seat_DAU,train_DAU->SQL_Connect,0,&seat,SEAT_tpl);
	DAU_init(&seat_remant_DAU,train_DAU->SQL_Connect,0,&remant,SEAT_REMANT_tpl);

	tnum=0;
	while(!DAU_next(train_DAU)) { //对于每一个车次
		jobday=today; //每次需要重新设定
		if(!dflg) { // 没有指定日期，按规定设置提前量
			if(train->days>0) jobday=today+train->days;
			else jobday+=getjobday(train_DAU,stmt);
		}
		if(train->beg_date > jobday || train->end_date < jobday)
				 continue;
		/* 计算席位母表周期 */
		i=(jobday-train->beg_date)%strlen(train->cycle);
		i_item=train->cycle[i];
		rjultostrfmt(day,jobday,YEAR_TO_DAY);
ShowLog(5,"xwfb gettrain:train_no=%s,beg_date=%s,end_date=%s,jobday=%s,i_item=%c",
	train->train_no, rjulstr(tmp,train->beg_date), rjulstr(tmp+100,train->end_date),day,i_item);
		if(i_item=='0') continue;

// 准备发布无号席位
		strcpy(stop_station.train_no,train->train_no);
		stop_station.beg_date=jobday;
		ret=stop_station_dao(&stop_station_DAU,stmt);
		if(ret<=0) {
		    if(train_DAU->SQL_Connect->Errno != SQLNOTFOUND)
			ShowLog(1,"xwfb 为无号查找停靠站表 stmt=%s, err=%d,%s",	stmt,
				train_DAU->SQL_Connect->Errno,
				train_DAU->SQL_Connect->ErrMsg);
		} else {
		  strcpy(seat.train_no,train->train_no);
		  strcpy(seat.gride,train->gride); // 如果无号的等级不是列车等级怎么办？
		  seat.end_station=train->station_num-1;
		  strcpy(seat.start_date,day);
		  seat.carno=0;
		  seat.purpose=0; //????
		  seat.seat_type=0;
		  seat.shortest_station=0;
		  seat.flag=0;	// 应该＝3，暂不可用
		  seat.used_time=now;
		  strcpy(seat.used_dev,"xwfb");
		  strcpy(seat.used_uid,seat.used_dev);

		  while(!DAU_next(&stop_station_DAU)) {//该车次的各站
		    strcpy(seat.beg_station,stop_station.station_code);
		    i=train->start_time+stop_station.running_time+
			stop_station.stop_time;
		    rminstrfmt(seat.on_date,jobday*1440+i/1440,YEAR_TO_MIN);
		    strcpy(seat.run_train,stop_station.run_train_no);
		    if(seat.end_station <= stop_station.sequence) {
			ShowLog(1,"无号：%s: 起止站错 seq=%d,station_num=%d",
				seat.train_no,stop_station.sequence,
				train->station_num);
			continue;
		    }
		    *seat.pro=0; //席位属性
		    DAU_copy(&seat_remant_DAU,&seat_DAU,0);
		    remant.amoung=0;
		    for(i=0;i<stop_station.over;i++) {
			seat.seat_no=i+1;
			ret=addxw(&seat_DAU,stmt);
		        if(!ret) remant.amoung++;
		    }
		    totalnum += remant.amoung;
ShowLog(2,"train=%s;%s,carno=%d,beg_station=%s,end_station=%d,remant=%d",
remant.on_date,remant.run_train,remant.carno,remant.beg_station,remant.end_station,remant.amoung);
		    if(!seat.flag && remant.amoung) ret=adj_remant(&seat_remant_DAU,stmt);
		  }
		}
// 发布普通席位
		pre_rec.beg_date=jobday;
		strcpy(pre_rec.train_no,train->train_no);
		*pre_rec.item=i_item;
		pre_rec.item[1]=0;
		p=DAU_mk_where(&pre_seat_DAU,"train_no,item",stmt);
		sprintf(p," AND beg_date <= :beg_date AND end_date >= :beg_date AND flag=0");
		if(0 >= DAU_select(&pre_seat_DAU,stmt,1)) continue;
//ShowLog(5,"xwfb:%s",stmt);
		DAU_next(&pre_seat_DAU);
		ret=mkxw(&pre_seat_DAU,stmt,day,train->start_time,carno);
		if(ret>0) totalnum+=ret;
		if(!(++tnum % 100))
			trans_commit(train_DAU->SQL_Connect);
	}
	DAU_free(&stop_station_DAU);
	DAU_free(&seat_DAU);
	DAU_free(&seat_remant_DAU);
	DAU_free(&pre_seat_DAU);
	trans_commit(train_DAU->SQL_Connect);
	ShowLog(2,"totalnum=%d",totalnum);

	return 0;
}

static char my_showid[128];
main(int argc,char *argv[])
{
int ret,i;
T_SQL_Connect SQL_Connect;
DAU train_DAU;
TRAIN_stu train;
char *carno=0;

	tzset();
	nice(10);
	nice(10);
	today=rtoday();
	sprintf(my_showid,"%s:%d",basename(argv[0]),getpid());
	Showid=my_showid;
	memset(train.train_no,0,sizeof(train.train_no));

/*******************************************************************
 *  get Opt
 *******************************************************************/
	for(i=1;i<argc;i++) {
		if(*argv[i]=='-') {
			switch(argv[i][1]) {
			case 'f':
				if(argv[i][2]) ret=envcfg(argv[i]+2);
				else {
					i++;
					ret=envcfg(argv[i]);
				}
				continue;
					
			case 'd':
				dflg=1;
			case 'D':
				if(argv[i][2]) today=cvtdate(&argv[i][2],today);
				else {
					i++;
					today=cvtdate(argv[i],today);
				}
				continue;
					
			case 'c':
				if(argv[i][2]) strncpy(train.train_no,
					&argv[i][2],sizeof(train.train_no)-1);
				else {
					i++;
					strncpy(train.train_no,
					     argv[i],sizeof(train.train_no)-1);
				}
				continue;
					
			case 'n':
				if(argv[i][2]) carno=&argv[i][2];
				else {
					i++;
					carno=argv[i];
				}
				continue;
					
			default:
				fprintf(stderr,"no know option:%s",argv[i]);
 				fprintf(stderr,"Usage:%s -f 配置文件 -d 日期表达式 -c 车次 -n 车厢号,车厢号,...",
					argv[0]);
				continue;
			}
		}
	}
/*********************************************************************
 *      Open DATABASE
 *********************************************************************/
	if(!*train.train_no) carno=0;
	train.beg_date=today;

	ret=db_open(&SQL_Connect);
	if(ret) {
		ShowLog(1,"Open Database err=%d.%s",
			SQL_Connect.Errno,
			SQL_Connect.ErrMsg);
		return 1;
	}
	DAU_init(&train_DAU,&SQL_Connect,0,&train,TRAIN_tpl);
	
	ret=xwfb(&train_DAU,carno);

	DAU_free(&train_DAU);
	___SQL_CloseDatabase__(&SQL_Connect);
	return(0);
}
