/*********************************************************
 * 18位身份证格式校验算法
 *********************************************************/
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char verify18card(char *card_no)
{
static const int weight[]={7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2,1};
static const char ver[]="10X98765432";
int i,j,check=0;
	for(i=0;i<17;i++) {
		if(!isdigit(card_no[i])) return 0;
		j=card_no[i]-'0';
		check+=j*weight[i];
	}
	return ver[check%11];
}

main()
{
char buf[20],c;
	while(!ferror(stdin)){
		printf("请输入身份证号码：\n");
		fgets(buf,sizeof(buf),stdin);
		if(feof(stdin)) break;
		if(strlen(buf)<19) {
err:
			printf("格式不对，重来！\n");
			continue;
		}
		c=verify18card(buf);
		if(c!=buf[17]) printf("校验错！\n");
		if(!c) goto err;
		printf("校验码：%c\n",c);

	}
	return 0;
}

