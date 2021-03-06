#include <arpa/inet.h>
#include <alloca.h>

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>

#include <limits.h>
#include <libgen.h>
#include <linux/sockios.h>

#include <math.h>

#include <netinet/in.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <netinet/if_ether.h>

#include <pthread.h>
#include <resolv.h>
#include <time.h> 

#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#include <signal.h>
#include <stdlib.h>

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/param.h>
#include <sys/ioctl.h> 
#include <sys/shm.h>
#include <sys/msg.h>

#define COLOR_NONE_C       "\033[m"
#define COLOR_RED          "\033[0;32;31m"
#define COLOR_LIGHT_RED    "\033[1;31m"
#define COLOR_GREEN        "\033[0;32;32m"
#define COLOR_LIGHT_GREEN  "\033[1;32m"
#define COLOR_BLUE         "\033[0;32;34m"
#define COLOR_LIGHT_BLUE   "\033[1;34m"
#define COLOR_DARY_GRAY    "\033[1;30m"
#define COLOR_CYAN         "\033[0;36m"
#define COLOR_LIGHT_CYAN   "\033[1;36m"
#define COLOR_PURPLE       "\033[0;35m"
#define COLOR_LIGHT_PURPLE "\033[1;35m"
#define COLOR_BROWN        "\033[0;33m"
#define COLOR_YELLOW       "\033[1;33m"
#define COLOR_LIGHT_GRAY   "\033[0;37m"
#define COLOR_WHITE        "\033[1;37m"

#define LOG_PERROR_INFO(first...) {\
    printf("[PID:%d]" COLOR_RED "[%s][%s][perror]:%d ", getpid(), "insert", __FUNCTION__, __LINE__);\
    printf(COLOR_RED first);\
    printf(COLOR_RED " errno = %d: %s\n", errno, strerror(errno));\
	printf(COLOR_NONE_C);\
    fflush(stdout);\
}
#define LOG_ERROR_INFO(first...) {\
    printf("[PID:%d]" COLOR_RED "[%s][%s][perror]:%d ", getpid(), "insert", __FUNCTION__, __LINE__);\
    printf(COLOR_RED first);\
	printf(COLOR_NONE_C);\
    fflush(stdout);\
}
typedef __CONST_SOCKADDR_ARG SA;

void socket_close(int socket)
{
	close(socket);
}

void get_nextval(char * ptn, int ptn_len, int * nextval)
{
	int i, j;

	nextval[0] = -1;
	for (i = 1; i < ptn_len; i ++)
	{
		j = nextval[i - 1];
		while (ptn[j + 1] != ptn[i] && (j >= 0))
		{
			j = nextval[j];
		}
		if (ptn[i] == ptn[j + 1])
		{
			nextval[i] = j + 1;
		}
		else
		{
			nextval[i] = -1;
		}
	}
}

int kmp_search(char * src, int src_len, char * patn, int patn_len, int const * nextval)
{
	int s_i = 0, p_i = 0;

	while (s_i < src_len && p_i < patn_len)
	{
		if (src[s_i] == patn[p_i])
		{
			s_i ++;
			p_i ++;
		}
		else
		{
			if (p_i == 0)
			{
				s_i++;
			}
			else
			{
				p_i = nextval[p_i - 1] + 1;
			}
		}
	}
	return (p_i == patn_len) ? (s_i - patn_len) : -1;
}
int find_str(char *psrc, int src_len, char *pfind, int find_len)
{
	char *src = psrc;
	char *ptn = pfind;
	int ptn_len = find_len;
	int nextval[ptn_len];
	get_nextval(ptn, ptn_len, nextval);
	return kmp_search(src, src_len, ptn, ptn_len, nextval);
}

/**Ìæ»»×Ö·û´®,ÒÔpstr³¤¶ÈÎª×¼*/
int replace(char *psrc, int src_len, char *prep, int rep_len, char *pstr, int str_len)
{
	char *p;
	int offset = find_str(psrc, src_len, prep, rep_len);
	if (offset >= 0)
	{
		p = psrc + offset;
		memcpy(p, pstr, str_len);

		return 0;
	}

	return -1;
}

int insert(char *psrc, int src_len, char *psub, int sub_len, char *pin, int pin_len)
{
	//char *p;
	//int size = 0;
	//int len = 0;
	int offset = find_str(psrc, src_len, psub, sub_len);
	if (offset >= 0)
	{
		char *pinsert_addr = psrc + offset + sub_len;
		char *pbackup_addr = pinsert_addr;
		int backup_len = psrc + src_len - pbackup_addr;
		char backup_buff[backup_len + 1];
		memcpy(backup_buff, pbackup_addr, backup_len);
		backup_buff[backup_len] = 0;
		memcpy(pinsert_addr, pin, pin_len);
		char *prebackup_addr = pinsert_addr + pin_len;
		memcpy(prebackup_addr, backup_buff, backup_len);
		*(prebackup_addr + backup_len) = 0;
		return offset;
	}
	return -1;
}

int repalce_with_pin(char *psrc, int src_len, char *psub, int sub_len, char *pin, int pin_len)
{
	int offset = find_str(psrc, src_len, psub, sub_len);
	if (offset >= 0)
	{
		char *preplace_addr = psrc + offset;
		char *pbackup_addr = preplace_addr + sub_len;
		int backup_len = psrc + src_len - pbackup_addr;
		char backup_buff[backup_len + 1];
		memcpy(backup_buff, pbackup_addr, backup_len);
		backup_buff[backup_len] = 0;

		memcpy(preplace_addr, pin, pin_len);
		char *prebackup_addr = preplace_addr + pin_len;
		memcpy(prebackup_addr, backup_buff, backup_len);
		*(prebackup_addr + backup_len) = 0;

		return 0;
	}

	return -1;
}

int isStrexit(char *src_buf, char *findstr)
{
	if (src_buf == NULL || findstr == NULL)
	{
		return  0;
	}
	int src_len = strlen(src_buf);
	int find_len = strlen(findstr);
	if (src_len <= 0 || find_len <= 0)
	{
		return 0;
	}
	int offset = find_str(src_buf, src_len, findstr, find_len);
	if (offset < 0)
	{
		return 0;
	}
	
	return 1;
}

int isStrnexit(char *src_buf, int src_len, char *findstr, int find_len)
{
	if (src_buf == NULL || findstr == NULL)
	{
		return  0;
	}
	if (src_len <= 0 || find_len <= 0)
	{
		return 0;
	}
	int offset = find_str(src_buf, src_len, findstr, find_len);
	if (offset < 0)
	{
		return 0;
	}
	
	return 1;
}

char *straddr(char *src_buf, char *findstr)
{
	if (src_buf == NULL || findstr == NULL)
	{
		return NULL;
	}
	int src_len = strlen(src_buf);
	int find_len = strlen(findstr);
	if (src_len <= 0 || find_len <= 0)
	{
		return NULL;
	}
	int offset = find_str(src_buf, src_len, findstr, find_len);
	if (offset < 0)
	{
		return NULL;
	}
	
	return src_buf + offset;
}

char *strnaddr(char *src_buf, int src_len, char *findstr, int find_len)
{
	if (src_buf == NULL || findstr == NULL)
	{
		return NULL;
	}

	int offset = find_str(src_buf, src_len, findstr, find_len);
	if (offset < 0)
	{
		return NULL;
	}
	
	return src_buf + offset;
}

int strline(char *psrc, char *pdest)
{
	int i = 0;
	for (i = 0; psrc[i] != '\0'; i ++)
	{
		pdest[i] = psrc[i];
		if (psrc[i] == '\n')
		{
			break;
		}
	}
	if (i == 0)
	{
		pdest[i] = '\0';
		return -1;
	}
	
	pdest[i+1] = '\0';
	return i+1;
}

int substr(char *text, int text_len, char *start, char *end, char *buf, int bufsize)
{
	char *title = start;
	int title_len = strlen(start);
	char *title_end = end;
	int title_end_len = strlen(end);
	int offset = find_str(text, text_len, title, title_len);
	if (offset >= 0)
	{
		char *title_start_addr = text + offset;
		char *find_addr = text + offset + title_len;
		if (title_start_addr)
		{
			offset = find_str(find_addr, text + text_len - find_addr, title_end, title_end_len);
			if (offset >= 0)
			{
				char *title_end_addr = find_addr + offset + title_end_len;
				if (title_end_addr)
				{
					int len = title_end_addr - title_start_addr;
					if (bufsize > len)
					{
						memcpy(buf, title_start_addr, len);
						return len;
					}
				}
			}
		}
	}

	return -1;
}

void free_malloc(char *m)
{
	if (m)
	{
		free(m);
		m = NULL;
	}
}

int get_addr_info(char *hostname, char *ipaddr)
{  
	struct addrinfo *answer, hint/*, *curr*/;
	char *ipstr = ipaddr;
	struct sockaddr_in *addr;
	bzero(&hint, sizeof(hint));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;

	int ret = -1;
	ret = getaddrinfo(hostname, NULL, &hint, &answer);
	if (ret != 0)
	{
		LOG_ERROR_INFO("[ %s ] getaddrinfo: errno = %d, %s\n", hostname, errno, gai_strerror(ret));
		if (answer)
		{
			freeaddrinfo(answer);
		}
		return -1;
	}

	//for (curr = answer; curr != NULL; curr = curr->ai_next)
	{
		if (! answer)
			return -1;
		
		addr = (struct sockaddr_in *)answer->ai_addr;
		inet_ntop(AF_INET, &addr->sin_addr, ipstr, 16);
		//break;
	}
	
	freeaddrinfo(answer);

	return 0;
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
    if (connect(clientfd, (SA) &serveraddr, sizeof(serveraddr)) < 0)
    {
    	close(clientfd);
		return -1;
	}
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

int get_url_path(char * url, char * host, char * path, int * port)
{
	int i, j = 0;
	int url_len = strlen(url);
	int offset = find_str(url, url_len, "http://", strlen("http://"));
	if (offset >= 0)
	{
		offset += strlen("http://");
	}
	else
	{
		offset = 0;
	}
	*port = 80;
	j = 0;
	for (i = offset; i < url_len; i ++)
	{
		if (url[i] == '/' || url[i] == ':') {
			break;
		}
		host[j ++] = url[i];
	}
	host[j] = 0;

	if (url[i] == ':')
	{
		char porttmp[20];
		j = 0;
		for (++ i; i < url_len; i ++)
		{
			if (url[i] == '/') {
				break;
			}
			porttmp[j ++] = url[i];
			if (j >= 20)
			{
				break;
			}
		}
		porttmp[j] = 0;
		if (j < 20 && port)
		{
			*port = atoi(porttmp);
		}
	}

	j = 0;
	for ( ; i < url_len; i ++)
	{
		path[j ++] = url[i];
	}
	if (j == 0)
	{
		path[j ++] = '/';
	}
	path[j] = 0;

	return 0;
}



int find_http_body(char * psrc, int src_len)
{
	int offset= 0;
	int len = 5;
	
	char *debug_0;
	char *psrctmp = psrc;
	offset = find_str(psrc, src_len, "<body", len);
	if (offset < 0) {
		return -1;
	}
	debug_0 = psrc + offset;
	
	offset += 5;
	psrctmp += offset;
	src_len -= offset;

	offset = find_str(psrctmp, src_len, ">", 1);
	if (offset < 0) {
		return -2;
	}
	psrctmp += offset + 1;
	char buf[psrctmp - debug_0 + 10];
	memcpy(buf, debug_0, psrctmp - debug_0);
	buf[psrctmp - debug_0] = 0;
	printf("%s\n\n", buf);
	return psrctmp - psrc;
}
int test_insert(char *url)
{
	char host[1024] = {0};
	char path[4096] = {0};
	int webport = -1;
	get_url_path(url, host, path, &webport);
	if (webport <= 0)
	{
		printf("error!\n");
		return -1;
	}
	char header[8192];
	sprintf(header, 
	"GET %s HTTP/1.1\r\n"
	"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
	"Accept-Rubbish!: gzip, deflate\r\n"
	"Accept-Language: zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3\r\n"
	"Host: %s\r\n"
	"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:47.0) Gecko/20100101 Firefox/47.0\r\n"
	"Connection: Close\r\n"
	"\r\n",
	path, host);
	int headerlen = strlen(header);
//printf("%s", header);
	int socket = connect_host_block(host, webport);
	if (socket <= 0)
	{
		LOG_PERROR_INFO("connect_host_block error");
		return -1;
	}
	int ret = send(socket, header, headerlen, 0);
	if (ret != headerlen)
	{
		LOG_PERROR_INFO("send error! ret = %d, headlen = %d", ret, headerlen);
		socket_close(socket);
		return -1;
	}
	char buf[1024 * 1024 + 10];
	int offset = 0;
	while (1)
	{
		char *buftmp = buf + offset;
 		ret = read(socket, buftmp, 8192);
		if (ret <= 0)
		{
			break;
		}
		offset += ret;
		buf[offset ++] = '\n';
		buf[offset ++] = '\n';
		buf[offset] = 0;
	}
	close(socket);
	if (find_http_body(buf, offset) < 0)
	{
		printf("%s", buf);
		return -99;
	}
	return 0;
}

int main(int args, char *argv[])
{
	if (args != 2)
	{
		printf("please input weburl, eg. %s %s\n", argv[0], "www.baidu.com");
		return 0;
	}
	int i = 1;
	while (1)
	{
		printf("%d -------------------------\n", i ++);
		if (test_insert(argv[1]) < 0)
		{
			break;
		}
		sleep(1);
	}
	return 0;
}