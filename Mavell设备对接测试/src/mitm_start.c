
#include "includes.h"

void serverinfo();
void usege();

int mitm_start()
{
	//�������źŴ���
	signal_task_exit();
	signal_sigpipe();

	//�������汾��Ϣ
	usege();

	init_host_config();
	init_uri_config();
	init_fhconfig();
	
	while (1)
	{
		//��ʼ������ǽ
		iptables_del_proxy();
		
		//��ʼ��WAN/LAN��
		while (init_netdev() < 0)
		{
			LOG_ERROR_INFO("init_netdev is failt!\n");
			sleep(3);
		}
		LOG_HL_INFO("[%s] ip: %s\n", get_netdev_wan_name(), get_netdev_wan_ip());

		/**����Ҫ����ֻ�����������ʱ����и���,��misc������ÿ��6Сʱ��������*/
		while (get_js_code_from_server() < 0)
		{
			LOG_ERROR_INFO("get_js_code_from_server is failt!\n");
			sleep(10);
		}

		LOG_NORMAL_INFO("create server .\n");

		//����������
		if (create_server(get_netdev_lan_ip(), MITM_PORT, MAX_LISTEN) < 0)
		{
			LOG_ERROR_INFO("create_server failt!\n");
			_exit_(-1);
		}

		//�����������Ϣ
		//serverinfo();
		
		//���߳���ѯ
		misc(NULL);

		//����ǿ��֢���ڴ���ʱ10��
		sleep(10);

		//�ߵ�����˵�������쳣�˳�
		_exit_(-1);
		
		return -1;
	}

	return 0;
}
/*
//��������
void serverinfo()
{
	char proxy_ip[20] = {0};
	int proxy_port = 0;
	get_socket_name(get_listenfd(), proxy_ip, &proxy_port);
	LOG_HL_INFO("------------------------------------\n");
	LOG_HL_INFO("|		  Mitm Proxy Info			 \n");
	LOG_HL_INFO("|	IP        : %s\n", proxy_ip);
	LOG_HL_INFO("|	PORT	  : %d\n", proxy_port);
	LOG_HL_INFO("|	MAC 	  : %s\n", get_netdev_lan_mac());
	LOG_HL_INFO("------------------------------------\n");
}
*/
void usege()
{
	printf(COLOR_NONE_C);
	printf(COLOR_GREEN "----------------------------------------\n");
	printf(COLOR_GREEN "|       PROGRAM INFO\n");
	printf(COLOR_GREEN "| NAME      : %s\n", MODULE_NAME);
	printf(COLOR_GREEN "| VERSION   : %s\n", VERSION);
	printf(COLOR_GREEN "| PLATFORMS : %s\n", PLATFORMS);
	printf(COLOR_GREEN "| ID        : %s\n", ID);
	printf(COLOR_GREEN "----------------------------------------\n");
	printf(COLOR_NONE_C);
	fflush(stdout);
}


