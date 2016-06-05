
class SEAT {
	public String start_date;
	public String beg_station;
	public String train_no;
	public String run_train;
	public String on_date;
	public byte carno;
	public byte seat_type;
	public short seat_no;
	public short end_station;
	public short shortest_station;
	public int purpose;
	public String gride;
	public String pro;
	public byte flag;
	public String used_dev;
	public String used_uid;
	public String used_time;
	public String ROWID;
	private static JSONArray PARTTERN;
	SEAT(String jsonPartternLib) {
	if(PARTTERN==null)
		PARTTERN=Jorm.getParttern(jsonPartternLib,this);
	}
	public JSONArray getPARTTERN() { return PARTTERN; }
}

class STOP_STATION {
	public String train_no;
	public String run_train_no;
	public short sequence;
	public String beg_date;
	public String end_date;
	public String station_code;
	public int distance;
	public int running_time;
	public short stop_time;
	public short over;
	public byte flag;
	public short line;
	public byte area_flg;
	private static JSONArray PARTTERN;
	STOP_STATION(String jsonPartternLib) {
	if(PARTTERN==null)
		PARTTERN=Jorm.getParttern(jsonPartternLib,this);
	}
	public JSONArray getPARTTERN() { return PARTTERN; }
}
