#include "includes.h"

void signal_exit(int signum)
{     
    int i;
    //���͸��ӽ����˳��ź�
    for (i = 0; i < PROCESS_NUM; i ++)
    {
    	if (g_ProcessArr[i] > 0)
    	{
        	printf("kill pid:%d\n", g_ProcessArr[i]);
      		kill(g_ProcessArr[i], SIGTERM);
		}
    }
  //pause();
	exit(0);
}


int signal_init()
{
    //���Ը��źţ����ѹرյ�socket����write��������ź�
    //������Ӵ˾䣬�����������ź� �ᱼ��
    signal_task_exit();
	signal_sigpipe();
    return 0;
}



