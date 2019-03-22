#include "includes.h"

void signal_task_exit()
{
	signal(SIGCHLD, SIG_IGN);
}

void signal_sigpipe()
{
	signal(SIGPIPE, SIG_IGN);
}

int main(int args, char *argv[])
{	
	signal_sigpipe();

	time_init();
	
	init_netdev();

	dev_static_info_init();
	
	qos_init(1024 * 1024 * 100);

	//��ʼ����Ϣ����
	InitMsgQueue();

	//create_udp_server_thread();
	evhttpd_create_server_thread();
		
	while (1)
	{
	    // ��Ϣ����
	    char recvbuf[MAX_MESSAGE_LEN + 1] = {0};
		int ret = RecvQchttpdMsg(recvbuf);
		if (ret > 0)
		{
            // ��Ϣ����
            LOG_NORMAL_INFO("recv data, len = %d\n", ret);
			OnMessage((unsigned char *)recvbuf, ret);
		}
	}

	return 0;
}




