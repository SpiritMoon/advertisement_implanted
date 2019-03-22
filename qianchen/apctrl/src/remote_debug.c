#include "includes.h"

/*************************************
	Զ�̿��ƣ�����ssh�������ʵ��
route:
cmd:
	ssh -fCNR <port_server_1>:localhost:22 user_server@IP_server
˵��:
	port_server_1: �������ϵĶ˿ڣ�������·������22�˿ڰ涨
	user_server: ��������ssh�ʻ�
	IP_server: ������IP
����:
	ssh -fCNR 8989:localhost:22 root@120.77.149.125

server:
cmd:
	ssh -fCNL *:<port_server_2>:localhost:<port_server_1> localhost
˵��:
	port_server_2: �������ϱ���ת���˿�������ͨ�ţ���������ת����port_server_1
	* : ��ʾ���������������
����:
	ssh -fCNL *:9090:localhost:8989 localhost

������:
cmd:
	ssh -p <port_server_2> user_route:IP_server
˵��:
	user_route: ·������ssh�ʻ�
����:
	ssh -p 9090 root@120.77.149.125
	
	
**************************************/


int router_ssh_to_server(char *server_ip, int server_port, char *user_server)
{
	char cmd_buf[255] = {0};
	EXECUTE_CMD(cmd_buf, "ssh -fCNR %d:localhost:22 %s@%s", server_port, user_server, server_ip);
	return 0;
}


