#include "includes.h"

#if (PROCESS_NUM > 1)
sprocess_lock *g_sprolock;					//������
#endif

int g_listenfd = -1;
int g_listen_port = 60000;

int server_init();

int create_server(char * bindip, int listen_port, int listen_counts)
{
	int listenfd = socket_listen(bindip, listen_port, listen_counts);
	if (listenfd < 0)
	{
		return -1;
	}
	g_listenfd = listenfd;
	g_listen_port = listen_port;
	
#if (PROCESS_NUM > 1)
	//����������
    g_sprolock = process_spinlock_create(sizeof(sprocess_lock));
    if (g_sprolock == (sprocess_lock *)-1)
    {
        LOG_ERROR_INFO("process lock create error, exit.!\n");
        return -1;
    }
#endif

#if 1
    //�����ӽ���
    int ret = process_CreateSub();
	if (1 == ret)
    {
    	//�ӽ���
        //��ʼ���źŴ���
        signal_init();
        LOG_NORMAL_INFO("child process running...\n");
    }
    else
    {
    	//������
    	close(listenfd);
    	return ret;
    }
#else
	signal_init();
#endif
	//--------�ӽ���ִ������-------------------------------

	//д��pid
	save_pid("mproxy_sub");

#if __SOCKET_MONITOR__	
	socket_monitor_init();
#endif

	if (server_init() < 0)
	{
		LOG_ERROR_INFO("server init failt! exit program.\n");
		socket_close(listenfd);
		_exit_(-1);
	}

	sleep(5);
	
	//����ǽ�ض���,����ѡ�����Σ�����������ƽ���qcdog
	//iptables_webview_proxy(get_netdev_lan_ip(), get_listen_port());
	while (msg_send_fwset(NULL, 0, FRAME_MODULE_MITM, FRAME_MODULE_FW_SET, FRAME_CMD_MITM_SET) < 0)
	{
		LOG_ERROR_INFO("msg_send_fwset failt! continue after 30 seconds ...\n");
		sleep(30);
	}

	printf("********** Init Ok **********\n");

	while (1)
	{
		display_worker_counts();
		display_client_counts();
		sleep(5);
	}

	return 0;
}

int server_init()
{
#ifdef _QC_THREAD_POOL_
	qc_threadwork_conf;
#else
	if (tpool_create(THREAD_NUM) < 0)
	{
		LOG_ERROR_INFO("create thread pool failt!\n");
		return -1;
	}
#endif

	if (listen_thread_start(g_listenfd) < 0)
	{
		return -1;
	}
	if (phone_thread_start() < 0)
	{
		return -1;
	}
	if (webserver_thread_start() < 0)
	{
		return -1;
	}
	
	return 0;
}

int epoll_server_exit()
{	
	signal_exit(SIGTERM);

	//��ӵ�һ���˳�
	char killall[125];
	sprintf(killall, "killall %s", MODULE_NAME);
	system(killall);

	int timecnt = 0;
	while (1)
	{
		sleep(1);
		timecnt ++;
		if (timecnt > 5)
		{
			//5���ڻ�δ�˳�
			LOG_WARN_INFO("KILLALL %s failt! now reboot system.\n", MODULE_NAME);
			//iptables_del_proxy();
			system("reboot -f");
		}
	}
	
	return 0;
}

int get_listenfd()
{
	return g_listenfd;
}

int get_listen_port()
{
	return g_listen_port;
}








