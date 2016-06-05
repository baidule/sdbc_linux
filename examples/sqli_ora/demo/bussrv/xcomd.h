#include <sdbc.h>
int event_put(T_Connect *conn,int Eno);
int Z_GetFile(T_Connect *connect,T_NetHead *NetHead);
int Z_PutFile(T_Connect *connect,T_NetHead *NetHead);
int busToServer(T_Connect *connect,T_NetHead *NetHead);
int serverToBus(T_Connect *connect,T_NetHead *NetHead);

typedef struct {
	char devid[41];
	char Showid[100];
} GDA;
