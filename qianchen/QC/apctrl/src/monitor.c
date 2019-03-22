#include "includes.h"



static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void monitor_wait_cond()
{
	pthread_cond_wait(&cond, &mut);
}

void monitor_send_cond()
{
	pthread_mutex_lock(&mut);
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mut);
}

int getPid(char *pro)
{
	FILE *fp = NULL;

	char cmd[255];
	sprintf(cmd, "ps | grep \"%s\"", pro);
	LOG_HL_INFO("%s\n", cmd);
	fp = popen(cmd, "r");
	if (NULL == fp)	
	{  
		LOG_PERROR_INFO("popen error!");  
		return -1;
	}

	char buf[512] = {0};
	while(fgets(buf, 255, fp) != NULL)
	{
		LOG_HL_INFO("%s", buf);
		if (isStrexit(buf, "grep"))
			continue;

		char tmp[255] = {0};
		if (awk(buf, " ", 0, tmp) >= 0)
		{	
			pclose(fp);
			return atoi(tmp);
		}
		
		//break;
	}
	pclose(fp);
	return -1;
}

int MonitorProcess(int pid)
{	
	char path[64] = {0};
	sprintf(path, "/proc/%d", pid);
	DIR* dir = opendir(path);
	if (dir == NULL)
	{
		LOG_ERROR_INFO("PID: %d , was exit !\n", pid);
		return -1;
	}
	closedir(dir);
	dir = NULL;
	return 0;
}

int monitor_runflag = 0;
void wait_monitor_exit()
{
	//1��ʾ��س����ڼ����
	if (monitor_runflag == 1)
	{
		//�ñ���Ǽ�س����for����
		//���Ϊ0��for�˳�
		monitor_runflag = 0;

		//2��ʾfor�˳���
		//�ȴ�����˳�
		while (monitor_runflag != 2)
		{
			sleep(1);
		}		
	}
}

int restart_flag = 0;
void *monitor_pid(void *arg)
{
	while (1)
	{
		monitor_wait_cond();

		//�Ȼ�ȡ���̵�pid
		int pid = getPid(MONITOR_PROGRAM);
		if (pid < 0)
		{
			//��ȡʧ�ܣ�˵������δ������
			LOG_ERROR_INFO("getPid failt .. !\n");

			//���ñ��λ��1��ʾ��Ҫ�����ܳ���, 0��ʾ����Ҫ�����ܳ���
			restart_flag = 1;

			//�ص�monitor_wait_cond�������߳������������
			continue;
		}

		LOG_WARN_INFO("Begin monitor ... \n");
		monitor_runflag = 1;

		//��ʼ����߳�״̬
		for ( ; monitor_runflag && MonitorProcess(pid) >= 0; )
			sleep(3);
		
		monitor_runflag = 2;
		
		LOG_SAVE(MONITOR_PROGRAM" exit !!!\n");

		//�ߵ�����˵�������˳�����Ҫ�������ǣ���֪���߳������ܳ���
		restart_flag = 1;
	}

	return NULL;
}

int create_monitor_pid_thread()
{
	pthread_t thread;
	if (pthread_create(&thread, NULL, monitor_pid, NULL) != 0)
	{
		LOG_PERROR_INFO("pthread_create error.");
		return -1;
	}
	return 0;
}

