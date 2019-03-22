#include "includes.h"

int connect_noblock(int clientfd, struct sockaddr_in *serveraddr)
{
	if (connect(clientfd, (SA *)serveraddr, sizeof(struct sockaddr_in)) >= 0) {
		//connect success
		return 0;
	}
#ifdef DEBUG_SOCKET	
	LOG_WARN_INFO("(s:%d)connect need to check. e:%d, %d, %d %s\r\n", 
					clientfd, errno, ENETUNREACH, EINPROGRESS, strerror(errno));
#endif

	// �����������ӣ���ʾ������������Ӧ��������ԭ����CONNECTʧ�ܣ���ֱ�ӷ��ش���
	// ���粻�ɵ����ʾ����������
	// ENETUNREACH : 101
	// EINPROGRESS : 115
	if ((errno != EINPROGRESS) || (errno == ENETUNREACH))
	{
		//���粻�ɴ����������
		return -1;
	}

	// ����Ƿ�������
	fd_set socketRed;
	fd_set socketWr;
	struct timeval sTimeout;
	int i = 0, ret = -1;
    socklen_t len;
    int iError = -1;
    
	while (1)
	{
		// ʵ�ʲ����У�ֱ�ӹػ��������ĵ�һ�����ӣ�һ��ᳬʱ��Ȼ�������ӵ�ʱ�򣬻�ܿ�������(�����ӡ�ļ�������Ϊ7)
		i ++;
		if (i > 20)
		{
#ifdef DEBUG_SOCKET
			LOG_WARN_INFO("(s:%d) connect time out.\r\n", clientfd);
#endif
			//��ʱ�˳�
			return -1;
		}
#ifdef DEBUG_SOCKET
		LOG_NORMAL_INFO("(s:%d) connect ...........................%d\r\n", clientfd, i);
#endif

		// ��select���ж��Ƿ�connect�ɹ�
		// ��ʼ��
		FD_ZERO(&socketRed);
		FD_ZERO(&socketWr);
		FD_SET (clientfd, &socketRed);
		FD_SET (clientfd, &socketWr);

		// �趨��ʱʱ��
		sTimeout.tv_sec  = 1;
		sTimeout.tv_usec = 0;

		// select
		ret = select(clientfd + 1, (fd_set*)(&socketRed), (fd_set*)(&socketWr), (fd_set*)NULL, &sTimeout);
		if (ret <= 0)
		{
			if (ret == 0)
			{
				//select��ʱ
				continue;
			}
			if (ret < 0)
			{
				//select����
				return -1;
			}
		}
		// �׽��ֿɶ�����д
		// ��Ҫ��һ���ж�
		if ((FD_ISSET(clientfd, &socketRed) > 0)
		 && (FD_ISSET(clientfd, &socketWr) > 0))
		{
#ifdef DEBUG_SOCKET		
			LOG_WARN_INFO("socket : %d read & write, need to check agin.\r\n", clientfd);
#endif
			len = sizeof(iError);
			ret = getsockopt(clientfd, SOL_SOCKET, SO_ERROR, &iError, &len);
			if (ret < 0)
			{
#ifdef DEBUG_SOCKET	
				LOG_ERROR_INFO("Get SO_ERROR error, connect failt.\r\n");
#endif
				// ��ȡSO_ERROR���ԣ�Ҳ����ʧ��
				return -1;
			}
#ifdef DEBUG_SOCKET
			LOG_ERROR_INFO("Get ERROR parma:%d, %d\r\n", iError, ret);
#endif

			if (iError != 0)
			{
#ifdef DEBUG_SOCKET
				LOG_ERROR_INFO("ERROR != 0; connect failt.\r\n");
#endif
				// ��ʾ���ӵ����ڻ�û�н���
				// return -1;

				continue;
			}

			if (0 == iError)
			{
#ifdef DEBUG_SOCKET			
				LOG_NORMAL_INFO("ERROR = 0; connect success.\r\n");
#endif
				// ��ʾ�����Ѿ�����
				return 0;
			}
		}

		// socket ��д���ɶ�����ʾ���ӽ���
		if ((FD_ISSET(clientfd, &socketRed) <= 0)
		 && (FD_ISSET(clientfd, &socketWr) > 0))
		{
#ifdef DEBUG_SOCKET
			LOG_NORMAL_INFO("socket:%d write & noread, connect success.\r\n", clientfd);
#endif
			return 0;
		}

		// socket �ɶ�����д����֪����ô��������ô������
		if ((FD_ISSET(clientfd, &socketRed) > 0)
		 && (FD_ISSET(clientfd, &socketWr) <= 0))
		{
#ifdef DEBUG_SOCKET
			LOG_ERROR_INFO("socket:%d read & nowrite, connect failt.\r\n", clientfd);
#endif
			// TCP socket ����Ϊ����������� connect ��connect �������û�����ϳɹ���
			// ���������� EINPROCESS��������жϷ���EINTR�� ���� TCP �� 3 �����ּ������С�
			// ֮������� select ��������Ƿ����ɹ����������ٴε���connect�������᷵��
			// ����EADDRINUSE��,�˴����鷵�ش��������³�ʼ������(��ʵ�ʲ��������ֱ��
			// ����1����continue���п��ܵ��³�����˳�).
			return -1;
		}
	}
	
	return -1;
}


int open_clientfd(char *bind_ip, char *hostname, int port)
{
    int clientfd;
    struct sockaddr_in serveraddr;
    struct sockaddr_in local_addr;
	char ip[20] = {0};
	
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		return -1; /* check errno for cause of error */
	}
	struct linger linger;
	linger.l_onoff = 0;
	linger.l_linger = 0;
	int optlen = sizeof(linger);
	setsockopt(clientfd, SOL_SOCKET, SO_LINGER, (char *)&linger, optlen);

    /* Fill in the server's IP address and port */
	if (get_addr_info(hostname, ip) < 0)
	{
		close(clientfd);
		return -2;
	}
	
	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ip);
	serveraddr.sin_port = htons(port);
	if (bind_ip)
	{
		bzero((char *) &local_addr, sizeof(local_addr));
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = inet_addr(bind_ip);
		local_addr.sin_port = 0;
		if (bind(clientfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) != 0)
		{
			LOG_PERROR_INFO("bind error");
			close(clientfd);
			return -1;
		}
	}
    /* Establish a connection with the server */
    if (connect(clientfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0)
    {
    	close(clientfd);
		return -1;
	}
    return clientfd;	
}

int open_clientfd_noblock(char *bindip, char *hostname, int port)
{
    int clientfd;
    struct sockaddr_in serveraddr;
	struct sockaddr_in local_addr;
	
    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		return -1;
	}

    char ipaddr[32] = {0};
    int ret = get_addr_info(hostname, ipaddr);
    if (ret == -1)
    {
    	close(clientfd);
		return -2;
	}
	else if (ret == -2)
	{
		close(clientfd);
		return -5;
	}
	
	struct linger linger;
	linger.l_onoff = 0;
	int optlen = sizeof(linger);
	setsockopt(clientfd, SOL_SOCKET, SO_LINGER, (char *)&linger, optlen);

	socket_setfd_noblock(clientfd);

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ipaddr);
	serveraddr.sin_port = htons(port);
	
	if (bindip)
	{
		bzero((char *) &local_addr, sizeof(local_addr));
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = inet_addr(bindip);
		local_addr.sin_port = 0;
		if (bind(clientfd, (struct sockaddr*)&local_addr, sizeof(local_addr)) != 0)
		{
			LOG_PERROR_INFO("bind error");
			close(clientfd);
			return -1;
		}
	}

	if (connect_noblock(clientfd, &serveraddr) < 0)
	{		
		close(clientfd);
		return -3;
	}
	
	socket_setfd_block(clientfd);

    return clientfd;
}

int connect_host_block(char *hostname, int port)
{
    int rc;
	
    if ((rc = open_clientfd(NULL, hostname, port)) < 0)
    {
		if (rc == -1)
		{
		    LOG_PERROR_INFO("Open_clientfd Unix error");
		}
		else
		{
			LOG_PERROR_INFO("%s:%d DNS ERROR\n", hostname, port);
		}
    }
    return rc;
}


int connect_host_noblock(char *hostname, int port)
{
    int rc;
	
    if ((rc = open_clientfd_noblock(NULL, hostname, port)) < 0)
    {
		if (rc == -1)
		{			
			LOG_ERROR_INFO("%s:%d socket create error!!!\n", hostname, port);
		}
		else if (rc == -2)
		{
			LOG_ERROR_INFO("%s:%d DNS ERROR!!!\n", hostname, port);
		}
		else if (rc == -3)
		{
			LOG_ERROR_INFO("%s:%d connect error!!!\n", hostname, port);		
		}
		else if (rc == -5)
		{
			LOG_ERROR_INFO("filter host : %s.\n", hostname);
		}
    }
    return rc;
}

int connecthost(char *hostname, int port, e_connectmode mode)
{
	int ret = -1;
	switch (mode)
	{
		case E_CONNECT_BLOCK:
			ret = connect_host_block(hostname, port);
			break;
		case E_CONNECT_NOBLOCK:
			ret = connect_host_noblock(hostname, port);
			break;
		default:
			break;
	}
	return ret;
}

ssize_t readline(int fd, void *usrbuf, size_t maxlen, socketstate_t *state)
{
    int n, rc = 0;
    char c[2] = {0}, *bufp = (char *)usrbuf;

    n = 0;
    if ((maxlen - 1) <= 0)
    {
    	*state = E_ERROR;
    	return -1;
	}
	
    while (n < maxlen - 1)
    {
		if ((rc = socket_read(fd, c, 1, state)) == 1)
		{
			n ++;
			*bufp ++ = c[0];
			if (c[0] == '\n')
			{
				break;
			}
		}
		else
		{
			break;
		}
    }
    *bufp = 0;

    return n;
}

