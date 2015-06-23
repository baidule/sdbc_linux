//���ؾ��⣬��һ����������ĳ�
resource * get_SC_weight()
{
int i,max_weight,w,n,m;
pool *pl;
struct timespec tims;
resource *rs;

	pthread_mutex_lock(&weightLock);
   do {
	max_weight=-1,n=-1,m=-1;
//	badtime=now_usec();
	pl=scpool;
	for(i=0;i<SCPOOLNUM;i++,pl++) {
//��Ȩ�����ص��Ǹ���
ShowLog(5,"%s:weight[%d]=%d",__FUNCTION__,i,pl->weight);
		pthread_mutex_lock(&pl->mut);
		if(pl->weight>0) {
			w=pl->weight<<9;
			w/=pl->resource_num;
			if(w>max_weight) {
				max_weight=w;
				n=i;
			}
		}
		pthread_mutex_unlock(&pl->mut);
	}
	if(n>=0) {
		rs=get_SC_resource(n,0);
		if(rs && rs != (resource *)-1) {
			pthread_mutex_unlock(&weightLock);
			return rs;
		}
		else continue;
	} 
	ShowLog(4,"%s:get_SC_weight:n=%d,weight=%d",__FUNCTION__,
				n,max_weight);
	for(m=0;m<SCPOOLNUM;m++) { //��һ����ϳ�,�����ܷ�ָ�
		if(scpool[m].weight>=0) continue;
		rs=get_SC_resource(m,1);
		if(rs && rs != (resource *)-1) {
			pthread_mutex_unlock(&weightLock);
			return rs;
		}
	}
	clock_gettime(CLOCK_REALTIME, &tims);
	tims.tv_sec+=6;//��Ϊ�黹���Ӳ�����weightLock�����ܶ�ʧ�¼�����6��
	pthread_cond_timedwait(&weightCond,&weightLock,&tims); //ʵ��û���ˣ���
	scpool_check();
    } while(1);
}
