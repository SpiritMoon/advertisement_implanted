/**
init ģ��: 
�����������
����������������: ssh(��Ϣת��)��qchttp(web����)

init��qchttpdģ�鲻���ڹ����ڴ棬ֻ����Ϣ����

*/

#include "includes.h"

void start_program(char *program)
{
	char cmd[255];
	EXECUTE_CMD(cmd, "%s &", program);
}

int main()
{
	signal(SIGPIPE, SIG_IGN);

	time_init();

	init_netdev();

	//��ʼ����Ϣ����
	InitMsgQueue();

	//��ʼ������������
	start_program("killall qchttpd;/etc/app/qchttpd");

	//��ȡdevNo
	request_dev_id_from_server_thread();

	//����update�߳�
	create_update_thread();
	
    while (1)
	{
	    // ��Ϣ����
	    char recvbuf[MAX_MESSAGE_LEN + 1]= {0};
		int ret = RecvInitMsg(recvbuf);
		if (ret > 0)
		{
            // ��Ϣ����
			OnMessage((unsigned char *)recvbuf, ret);
		}
    }
	
	return 0;
}

