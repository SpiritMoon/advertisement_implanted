#include "includes.h"

typedef struct sockaddr SA;

int socket_setfd_noblock(int sockfd)
{
    int flag = fcntl(sockfd, F_GETFL, 0);
    if (flag < 0)
    {
		LOG_PERROR_INFO("fcntl F_GETFL error");
		return -1;
    }
	if (fcntl(sockfd, F_SETFL, (flag | O_NONBLOCK)) < 0)
	{
		LOG_PERROR_INFO("fcntl F_SETFL error");
		return -1;
	}
	
	return 0;
}

int socket_setfd_block(int sockfd)
{
    int flag = fcntl(sockfd, F_GETFL, 0);
    if (flag < 0)
    {
		LOG_PERROR_INFO("fcntl F_GETFL error");
		return -1;
    }
	if (fcntl(sockfd, F_SETFL, (flag & ~O_NONBLOCK)) < 0)
	{
		LOG_PERROR_INFO("fcntl F_SETFL error");
		return -1;
	}
	
	return 0;
}

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

int socket_read(int sockfd, char *buf, int len)
{
	return read(sockfd, buf, len);
}

int socket_send(int sockfd, char *buf, int len)
{
	return send(sockfd, buf, len, 0);
}

int readline(int fd, void *usrbuf, int maxlen)
{
    int n, rc = 0;
    char c[2] = {0}, *bufp = (char *)usrbuf;

    n = 0;
    if ((maxlen - 1) <= 0)
    {
    	return -1;
	}
	
    while (n < maxlen - 1)
    {
		if ((rc = read(fd, c, 1)) == 1)
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

int read_header(int fd, char * buffer, int buffer_len, int *read_end)
{  
	int total_read = 0;
	int read_len = buffer_len;
	if (read_end)
	{
		*read_end = 0;
	}
	char *line_buffer = buffer;
    while (read_len > 0)
    {
		if (read_len > MAXLINE)
		{
			read_len = MAXLINE;
		}
        int ret = readline(fd, line_buffer, read_len);
        if (ret <= 0)
        {
            break;
        }
        total_read += ret;
        read_len = buffer_len - total_read;
		
        //�����˿��У�httpͷ����
        if (0 < ret && ret <= 2)
        {
	        if (strcmp(line_buffer, "\r\n") == 0)
	        {
				if (read_end)
				{
					*read_end = 1;
				}
	        	break;
	        }
        }
        
		line_buffer += ret;
    }
    if (total_read <= 0)
    {
		return -1;
    }
    
    buffer[total_read] = '\0';

    return total_read;
}


int readChunkedTextToFile(int socket, char *file)
{
	int readed_len = 0;
	char chunked[50];
	char buf[MAXBUF+1];
	int fd = open(file, O_RDWR | O_CREAT | O_TRUNC);
	if (fd <= 0)
	{
		LOG_PERROR_INFO("open \"%s\" error!", file);
		return -1;
	}
	
	while (1)
	{
		int offset = 0;
		memset(chunked, 0, 50);
		for ( ; ; )
		{
			char ch;
			int len = read(socket, &ch, 1);
			if (len != 1)
			{
				perror("read chunked size error.");
				goto _return_error_;
			}
			if (ch == '\r'
			|| ch == '\n')
				break;
			chunked[offset ++] = ch;
		}
		int contentLen = strtol(chunked, NULL, 16);
		LOG_NORMAL_INFO("[+] chunked size = %d\n", contentLen);
		if (contentLen == 0)
		{
			goto _return_;
		}
		else if (contentLen < 0)
		{
			goto _return_error_;
		}
		char tmp[2];
		if (read(socket, tmp, 1) <= 0)
		{
			LOG_PERROR_INFO("read chunked flag error.");
			goto _return_error_;
		}
		int readlen = 0;
		while (contentLen > 0)
		{
			if (contentLen > MAXBUF)
			{
				readlen = MAXBUF;
			}
			else
			{
				readlen = contentLen;
			}
			int len = read(socket, buf, readlen);
			if (len <= 0)
			{
				LOG_PERROR_INFO("read chunked text error.");
				goto _return_error_;				
			}
			buf[len] = '\0';
			contentLen -= len;
			readed_len += len;
			if (write(fd, buf, len) != len)
			{
				LOG_PERROR_INFO("write error.");
				goto _return_error_;
			}
		}
		if (read(socket, tmp, 2) <= 0)
		{
			LOG_PERROR_INFO("read chunked flag error.");
			goto _return_error_;
		}
	}
_return_:
	close(fd);
	sync();
	return readed_len;
_return_error_:
	close(fd);
	return -1;
}

