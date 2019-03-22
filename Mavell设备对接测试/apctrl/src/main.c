#include "includes.h"

int MonitorProcess()
{
	FILE *fp = NULL;

	char cmd[255];
	sprintf(cmd, "ps | grep \"mproxy_arm\" | wc -l");
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

	//�������ն�ִ��ps | grep \"mproxy_arm\" | wc -lʱ���ӡ����������
	//��popenִ�еĲ�һ����popenִ�е�Ҫ���1
	if (atoi(buf) <= 2)
	{
		return -1;
	}
	return 0;
}

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

	while (1)
	{
		cur_time = GetSystemTime_Sec();
		if (download_flag || abs(cur_time - old_time) > UPDATE_TIME)
		{
			download_flag = 0;
			old_time = cur_time;
			int ret = RequestUpdate();
			if (ret > 0)
			{
				LOG_SAVE("update success, restart program .\n");
				if (start_local_program() < 0)
				{
					//����ʧ�ܣ������ڸó���
					LOG_ERROR_INFO("local program isnot exsiting !\n");
				
					//����汾��
					memset(version, 0, sizeof(version));
					
					//��������
					download_flag = 1;
				}
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
		if (MonitorProcess() < 0)
		{
			LOG_SAVE("mproxy_arm process is exit!\n");

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
			sleep(1);
		}
		sleep(3);
	}

	return 0;
}



