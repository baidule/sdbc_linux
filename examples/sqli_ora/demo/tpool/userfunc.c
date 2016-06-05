srvfunc Function[]={
        {login,"login"},        /*0 login.c */
        {Echo,"Echo"},                  /*1*/
        {Rexec, "Rexec"},
        {GetFile,"GetFile"},
        {PutFile,"PutFile"},
        {Pwd,"Pwd"},        
        {filels,"filels"},
        {transaction_svc,"SQL_EndTran"},
	{select_svc,"SQL_Select"},
	{prepare_svc,"SQL_Prepare"},
	{SQL_Fetch,"SQL_Fetch"},
	{close_svc,"SQL_Close"},
	{exec_svc,"SQL_Exec"},
	{rpc_svc,"SQL_RPC"},
	{page_select,"page_select"},
	{dui_svc,"dui"},
	{cliToServer,"cliToServer"},
	{serverToCli,"serverToCli"},
	{ctx_login_svc,"ctx_login_svc"},
	{ctx_logout_svc,"ctx_logout_svc"},
	{net_showlog,"net_showlog"},
	{0,0}
};
