#include "includes.h"
#if 0
//�ϴ����������
int limit_speed_upload(int who, int speed)
{
	char iptables[512];
	sprintf(iptables, "iptables -t mangle -I PREROUTING 1 -i %s -w -p tcp -s %s.%d -m limit --limit %d/s -j ACCEPT",
					get_netdev_lan_name(), get_netdev_lan_segment(), who, speed);
	system(iptables);
	return 0;
}

//���ش��������
int limit_speed_download(int who, int speed)
{
	char iptables[512];
	sprintf(iptables, "iptables -t mangle -I POSTROUTING 1 -o %s -w -p tcp -d %s.%d -m limit --limit %d/s -j ACCEPT",
					get_netdev_lan_name(), get_netdev_lan_segment(), who, speed);
	system(iptables);	
	return 0;
}

//��������
int drop_all_upload_package()
{
	char iptables[512];
	sprintf(iptables, "iptables -t mangle -I PREROUTING 1 -i %s -w -p tcp -s %s.0/24 -j DROP",
					get_netdev_lan_name(), get_netdev_lan_segment());
	system(iptables);
	return 0;
}

//��������
int drop_all_download_package()
{
	char iptables[512];
	sprintf(iptables, "iptables -t mangle -I POSTROUTING 1 -o %s -w -p tcp -d %s.0/24 -j DROP",
					get_netdev_lan_name(), get_netdev_lan_segment());
	system(iptables);
	return 0;
}

//������֤��ص�IP��ַ
int accept_auth_server()
{
	//����
	return 0;
}

int limit_speed()
{
	
	
	return 0;
}

#endif
