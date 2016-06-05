create or replace 
function  ylhtest (
	start_date in varchar2,
	end_date in varchar2,
	org_code in tjrb.unit%TYPE,
	i_tabname in tjrb.tabname%TYPE,
	i_flg in tjrb.flg%TYPE
) return pkg_result.ora_result
as
res pkg_result.ora_result;
begin
	open res for
	    select * from tjrb
	    where tjdate >= to_date(start_date,'YYYY-MM-DD')
		and tjdate <= to_date(end_date,'YYYY-MM-DD')
		and unit = org_code 
		and tabname = i_tabname 
		and flg = i_flg;
	return res;
end;
/
