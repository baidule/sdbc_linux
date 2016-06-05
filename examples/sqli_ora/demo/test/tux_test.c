#include<stdio.h>
#include<string.h>
#include<atmi.h>
#include<json.h>
//typedef JSON_OBJECT ;

int main()
{
		char *sendbuf, *rcvbuf;
    long sendlen, rcvlen;
    int ret;
    char req_str[4096];
		json_object *json_req;
		json_object *json_head,*json_param;
		
		json_req=json_object_new_object();
		
		json_head=json_object_new_object();
		json_object_object_add(json_head,"op",json_object_new_string("get_price"));
		json_object_object_add(json_head,"session_id",json_object_new_string("100010"));
		json_object_object_add(json_head,"station_code",json_object_new_string("100157"));
		json_object_object_add(json_head,"bureau_code",json_object_new_string("1000"));
		json_object_object_add(json_req,"head",json_head);
		
		json_param=json_object_new_object();
		json_object_object_add(json_param,"tkt_type",json_object_new_string("1"));
   	json_object_object_add(json_param,"seat_type",json_object_new_string("O"));
   	json_object_object_add(json_param,"bed_type",json_object_new_string("0"));
   	json_object_object_add(json_param,"trn_id",json_object_new_string("24000C227102"));
   	json_object_object_add(json_param,"start_stn",json_object_new_string("10010"));
   	json_object_object_add(json_param,"end_stn",json_object_new_string("10172"));
   	json_object_object_add(json_param,"beg_date",json_object_new_string("2009-09-19"));
   	json_object_object_add(json_param,"end_date",json_object_new_string("2009-09-19"));
   	json_object_object_add(json_req,"param",json_param);
   	
   	strcpy(req_str,json_object_to_json_string(json_req));
   	
   	json_object_put(json_req);
   	
   	sendlen=strlen(req_str);
		

		if(tpinit((TPINIT *) NULL) == -1) {
        (void) fprintf(stderr, "Tpinit failed\n");
        exit(1);
    }

   

    /* Allocate STRING buffers for the request and the reply */

    if((sendbuf = (char *) tpalloc("STRING", NULL, sendlen+1)) == NULL) {
        (void) fprintf(stderr,"Error allocating send buffer\n");
        tpterm();
        exit(1);
    }

    if((rcvbuf = (char *) tpalloc("STRING", NULL, sendlen+1)) == NULL) {
        (void) fprintf(stderr,"Error allocating receive buffer\n");
        tpfree(sendbuf);
        tpterm();
        exit(1);
    }

    (void) strcpy(sendbuf, req_str);

    /* Request the service TOUPPER, waiting for a reply */
    ret = tpcall("get_price", (char *)sendbuf, 0, (char **)&rcvbuf, &rcvlen, (long)0);

    if(ret == -1) {
        (void) fprintf(stderr, "Can't send request to service TOUPPER\n");
     		(void) fprintf(stderr, "Tperrno = %d\n", tperrno);
        tpfree(sendbuf);
        tpfree(rcvbuf);
        tpterm();
        exit(1);
    }
		(void) fprintf(stdout, "Returned string is: %s\n", rcvbuf);

    /* Free Buffers & Detach from System/T */
    tpfree(sendbuf);
    tpfree(rcvbuf);
    tpterm();
    return(0);
}