#include "includes.h"


/**
AC����APͨ���㲥
AC��������APͨ��UDP
*/

int ac_process(int ufd, struct sockaddr_in *from, char *buf, int len)
{
	LOG_HL_INFO("len: %d, buf: %s\n", len, buf);

	char brdaddr[32] = {0};
	get_dev_bcast(brdaddr, LAN_DEV);
	udp_send_to(ufd, buf, len, brdaddr, AP_CLIENT_PORT);
	
	return 0;
}



