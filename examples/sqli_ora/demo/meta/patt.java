
class PATTERN_COL {
	public String tab_name;
	public String col_name;
	public String col_type;
	public int col_len;
	public String col_format;
	public String pseudo_name;
	private static JSONArray PARTTERN;
	PATTERN_COL(String jsonPartternLib) {
	if(PARTTERN==null)
		PARTTERN=Jorm.getParttern(jsonPartternLib,this);
	}
	public JSONArray getPARTTERN() { return PARTTERN; }
}
