/**
monitor ģ��: 
��������ػ�
*/

#include "includes.h"

int main()
{
	signal(SIGPIPE, SIG_IGN);
	
	//��ʼ����Ϣ����
	InitMsgQueue();

	create_monitor_mproxy_thread();
	
    while (1)
	{
	    // ��Ϣ����
	    // ��Ϣ��������:����ģ��Ե������ġ��ֱ���ѯ�����õ���Ӧ
	    char recvbuf[MAX_MESSAGE_LEN + 1]= {0};
		int ret = RecvMonitorMsg(recvbuf);
		if (ret > 0)
		{
            // ��Ϣ����
            LOG_NORMAL_INFO("recv data, len = %d\n", ret);
			OnMessage((unsigned char *)recvbuf, ret);
		}
    }
	
	return 0;
}

