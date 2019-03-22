#include "includes.h"

int init_firewall()
{
#if 0
	//֮ǰ��������֤�����˷���ǽ
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
	//��ʼ������ǽ,Ŀ����Ϊ�˾������û��ָܻ�����
	init_firewall();

	//kill���ϳ���
	system("killall "MONITOR_PROGRAM);

	//����Ƿ���ڳ���
	if ((access(FILE_PATH"/"MONITOR_PROGRAM, F_OK)) == 0)
	{
		//ִ�г���
		system(FILE_PATH"/"MONITOR_PROGRAM" &");
		return 0;
	}

	//δ��⵽�г������
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
		//ÿ��һ��Сʱ����һ�Σ����߸��±�ǿ���Ҳ���и���
		if (/*download_flag || */abs(cur_time - old_time) > 1 * _1_HOUR_)
		{
			//�����ǲ�����ʱ�䴥��������download_flag����
			//��Ҫ��download_flag���
			download_flag = 0;

			//��ֵ��ǰʱ��,�����´μ���
			old_time = cur_time;

			//��ʼ������£�����0��ʾ������£�1��ʾ���³ɹ���<0��ʾ���³���
			int ret = RequestUpdate();
			if (ret > 0)
			{
				//���³ɹ���׼�����������µĸ��°�
				LOG_SAVE("update success, restart program .\n");
				restart_flag = 1;
				sleep(1);
			}
			else if (ret == 0)
			{
				//�������,��1Сʱ���ٴθ���
				download_flag = 0;
			}
			else
			{
				//����ʧ��,��С��ʱ�����ٴ�ȥ����
				download_flag = 1;
			}
		}

		//���³ɹ�����Ҫkill���ϳ������������³���
		if (restart_flag)
		{
			//ֻ����һ��
			restart_flag = 0;

			//�ȴ�����߳��˳����ģʽ
			wait_monitor_exit();
			
			LOG_SAVE("mproxy_arm process restart ... \n");

			//��������
			if (start_local_program() < 0)
			{
				//����ʧ�ܣ������ڸó���
				LOG_ERROR_INFO("local program isnot exsiting !\n");

				//����ʧ�ܺ�����汾�ţ��汾�����ں�̨�Աȣ����δ���
				//������ɺ�̨��Ϊ·�����Ѿ������³��򣬲����·����°�
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



