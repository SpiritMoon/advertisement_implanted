/**
fw_set ģ��: 
�������ǽ������
*/

#include "includes.h"

int main()
{
	signal(SIGPIPE, SIG_IGN);

	//��ʼ�������ڴ�
	dev_share_init();

	//��ʼ����Ϣ����
	InitMsgQueue();

	//��ʼ������ǽ
	fw_init();
	
    while (1)
	{
	    // ��Ϣ����
	    // ��Ϣ��������:����ģ��Ե������ġ��ֱ���ѯ�����õ���Ӧ
	    char recvbuf[MAX_MESSAGE_LEN + 1]= {0};
		int ret = RecvFwSetMsg(recvbuf);
		if (ret > 0)
		{
            // ��Ϣ����
            LOG_NORMAL_INFO("recv data, len = %d\n", ret);
			OnMessage((unsigned char *)recvbuf, ret);
		}
    }
	
	return 0;
}

