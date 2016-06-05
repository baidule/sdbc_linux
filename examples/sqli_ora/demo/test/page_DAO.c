#include <stdio.h>
#include <string.h>

/*

SELECT * FROM
(
SELECT A.*, ROWNUM RN
FROM (SELECT * FROM TABLE_NAME) A
WHERE ROWNUM <= 40
)
WHERE RN >= 21
*/

char *get_page(char *stmt,int page_no,int page_size)
{
char *p;
int min,max;
	min=page_no*page_size;
	max=min+page_size;
	p=strdup(stmt);
	sprintf(stmt,"select * FROM (select A.*,ROWNUM RN FROM (%s) A WHERE ROWNUM <= %d) WHERE RN > %d",
		p,max,min);

	free(p);
	return stmt;
}
