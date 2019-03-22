#include "includes.h"

int init_firewall()
{
#if 1
	FILE *fp = NULL;

	char cmd[255];
	sprintf(cmd, "iptables -t mangle -nL seaving | wc -l");
	LOG_HL_INFO("%s\n", cmd);
	fp = popen(cmd, "r");
	if (NULL == fp)	
	{  
		LOG_PERROR_INFO("popen error!");  
		return -1;
	}

	char buf[20] = {0};
	while(fgets(buf, 20, fp) != NULL)
	{
		LOG_HL_INFO("%s", buf);
		//break;
	}
	pclose(fp);
	int i;
	for (i = 0; buf[i] != '\0'; i ++)
	{
		if (buf[i] == '\r'
		|| buf[i] == '\n')
		{
			buf[i] = '\0';
			break;
		}
	}	
	if (atoi(buf) <= 0)
	{
		return -1;
	}
#endif
	system("/etc/init.d/firewall restart");
	return 0;
}

int start_local_program()
{
	init_firewall();
	system("killall mproxy_arm");
	if ((access(FILE_PATH"/mproxy_arm", F_OK)) == 0)
	{
		system(FILE_PATH"/mproxy_arm &");
		return 0;
	}
	return -1;
}

extern int restart_flag;
int main(void)
{
	int cur_time = 0;
	int old_time = 0;
	int download_flag = 0;
	
	signal(SIGPIPE, SIG_IGN);

	//��ʼ��WAN/LAN��
	while (init_netdev() < 0)
	{
		LOG_ERROR_INFO("init_netdev is failt!\n");
		sleep(3);
	}
	LOG_HL_INFO("[%s] ip: %s\n", get_netdev_wan_name(), get_netdev_wan_ip());

	//ͬ��ʱ��
	int year = 0;
	while (year < 16)
	{
		LOG_WARN_INFO("wait system time sync ...\n");
		GetSystemTime(&year, NULL, NULL, NULL, NULL);
		sleep(3);
	}

//ʱ��ͬ���ɹ�,���´�����¼�������־
#if LOG_SAVE_ON
	dup_file();
#endif

	//����߳�
	create_monitor_pid_thread();

	while (1)
	{
		cur_time = GetSystemTime_Sec();
		if (download_flag || abs(cur_time - old_time) > 1 * _1_HOUR_)
		{
			download_flag = 0;
			old_time = cur_time;
			int ret = RequestUpdate();
			if (ret > 0)
			{
				LOG_SAVE("update success, restart program .\n");
				restart_flag = 1;
				sleep(1);
			}
			else if (ret == 0)
			{
				//�������
				download_flag = 0;
			}
			else
			{
				//����ʧ��
				download_flag = 1;
			}
		}
		if (restart_flag)
		{
			restart_flag = 0;
		
			wait_monitor_exit();
			
			LOG_SAVE("mproxy_arm process restart ... \n");

			//��������
			if (start_local_program() < 0)
			{
				//����ʧ�ܣ������ڸó���
				LOG_ERROR_INFO("local program isnot exsiting !\n");

				//����汾��
				memset(version, 0, sizeof(version));

				//��������
				download_flag = 1;
			}
			else
			{
				//��ʼ���
				monitor_send_cond();
			}
			
			sleep(1);
		}
		sleep(3);
	}

	return 0;
}



