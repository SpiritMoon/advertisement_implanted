/**
apctrl ģ��: 
�����������
����������������: ssh(��Ϣת��)��qchttp(web����)

init��qchttpdģ�鲻���ڹ����ڴ棬ֻ����Ϣ����

init��������:
���ذ�����ѹ�������нű�

�ű�����:
���� apctrl�Լ������ĳ���

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
	
	//��ʼ���豸��ع����ڴ�
	if (dev_share_init() < 0)
		exit(-1);

	//��ʼ����Ϣ����
	InitMsgQueue();

    while (1)
	{
	    // ��Ϣ����
	    // ��Ϣ��������:����ģ��Ե������ġ��ֱ���ѯ�����õ���Ӧ
	    char recvbuf[MAX_MESSAGE_LEN + 1]= {0};
		int ret = RecvApctrlMsg(recvbuf);
		if (ret > 0)
		{
            // ��Ϣ����
			OnMessage((unsigned char *)recvbuf, ret);
		}
    }
	
	return 0;
}

