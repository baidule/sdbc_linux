#include <pack.h>
#include <json_pack.h>

#include "meta/st.tpl"
#include "meta/st.stu"

main()
{
ss_stu ss;
char buf[512];
JSON_OBJECT json=json_object_new_object();

   ss.c=1;
   ss.s.c=3;
   ss.s.bi=123456789012345;
   ss.i=12345;
	stu_to_json(json,&ss,ss_tpl,0,0);
	printf("json ss=%s\n",json_object_to_json_string(json));
	net_pack(buf,&ss,ss_tpl);
	printf("ss=%s\n",buf);

   json_object_put(json);
   return 0;
}
