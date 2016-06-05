select 'ABCDSF' from dual;
declare
cur ref cursor;
begin
cur := ylhtest('2007-01-01','2007-01-02','00','A1',102);
print 'AAAA';
print :cur;
end;
