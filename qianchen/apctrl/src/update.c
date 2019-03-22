#include "includes.h"

#define UPDATE_HOST		"www.qcwifi.ltd"
#define UPDATE_HOST_1	"http://" UPDATE_HOST
#define UPDATE_PORT		80

#define SHELL_FILE		"/tmp/app/start.sh"
#define UPDATE_PATH		"/tmp/app.tar"
#define VERSION_TEXT	"/tmp/app/version.txt"

int check_result(char *res)
{
	LOG_NORMAL_INFO("%s\n", res);
	if (strcmp(res, "000") == 0)
	{
		//����
		return 1;
	}
	if (strcmp(res, "001") == 0)
	{
		LOG_ERROR_INFO("Firmware version inconsistent!\n");
		return -1;
	}
	if (strcmp(res, "002") == 0)
	{
		LOG_ERROR_INFO("Gcc version inconsistent!\n");
		return -1;
	}
	if (strcmp(res, "003") == 0)
	{
		LOG_ERROR_INFO("Firmware is the latest!\n");

		//�Ѿ������£��������
		return 0;
	}
	if (strcmp(res, "777") == 0)
	{
		LOG_ERROR_INFO("system error!\n");
		return -1;
	}
	if (strcmp(res, "999") == 0)
	{
		LOG_ERROR_INFO("invalid device!\n");
		return -1;
	}

	return -1;
}

//1 ���سɹ���0 ������£� -1 ����ʧ��
int download(int sockfd)
{
	char buf[MAXBUF + 1] = {0};
	
	int ret = read_header(sockfd, buf, MAXBUF, NULL);
	if (ret <= 0)
	{
		LOG_ERROR_INFO("read update header error!\n");
		return -1;
	}
	LOG_NORMAL_INFO("%s", buf);

	//�ж��Ƿ���chunked����content-length
	int len = 0;
	int contentLen = get_content_length(buf, ret);
	memset(buf, 0, MAXBUF);
	if (contentLen < 0)
	{
		//���ն�̬������
		LOG_WARN_INFO("can't find content length, try chunked ... \n");
		len = readChunkedText(sockfd, buf, MAXBUF);
		if (len <= 0)
		{
			LOG_ERROR_INFO("readChunkedText failt!\n");
			return -1;
		}
	}
	else if (contentLen > 0)
	{
		if (contentLen > MAXBUF) {
			contentLen = MAXBUF;
		}
		len = readStaticText(sockfd, contentLen, buf);
		if (len <= 0)
		{
			LOG_ERROR_INFO("readStaticText failt!\n");
			return -1;
		}
	}
	else
	{
		LOG_ERROR_INFO("content len == 0 !\n");
		return -1;
	}
	LOG_HL_INFO("%s\n", buf);
	
	//��ʼ���س���,д�뵽�ļ�
	char *updatefile = UPDATE_PATH;
	unlink(updatefile);
	int offset = 0;

	//״̬��
	char code[33] = {0};
	ret = strline(buf+offset, code, sizeof(code));
	if (ret <= 0)
	{
		return -1;
	}
	offset += ret;
	code[ret] = '\0';
	del_tab_newline(code);
	
	//�жϽ��
	if ((ret = check_result(code)) <= 0)
	{
		return ret;
	}

	//���md5
	char plugin_md5[36] = {0};
	ret = strline(buf+offset, plugin_md5, sizeof(plugin_md5));
	if (ret <= 0)
	{
		return -1;
	}
	offset += ret;
	plugin_md5[ret] = '\0';
	del_tab_newline(plugin_md5);
	LOG_NORMAL_INFO("plugin md5: %s, offset: %d\n", plugin_md5, offset);

	//���ص�ַ
	char download_url[1024] = {0};
	ret = strline(buf+offset, download_url, sizeof(download_url));
	if (ret <= 0)
	{
		return -1;
	}
	offset += ret;
	download_url[ret] = '\0';
	del_tab_newline(download_url);
	LOG_NORMAL_INFO("download_url: %s, offset: %d\n", download_url, offset);

	//�������ݶε�md5
	char res_md5[36] = {0};
	ret = strline(buf+offset, res_md5, sizeof(res_md5));
	if (ret <= 0)
	{
		return -1;
	}
	offset += ret;
	res_md5[ret] = '\0';
	del_tab_newline(res_md5);
	LOG_NORMAL_INFO("res_md5: %s, offset: %d\n", res_md5, offset);

	sprintf(buf, "%s%s", plugin_md5, download_url);
	char cal_md5[33] = {0};
	CalMd5(buf, strlen(buf), cal_md5);
	if (strcmp(cal_md5, res_md5))
	{
		LOG_ERROR_INFO("result md5 error! res:%s, cal:%s\n", res_md5, cal_md5);
		return -1;
	}

	char dhost[256];
	char duri[256];
	int dport = 80;
	ret = get_url_path(download_url, dhost, duri, &dport);
	if (ret < 0)
	{
		LOG_ERROR_INFO("get_url_path ERROR!\n");
		return -1;
	}

	int downfd = open_clientfd_noblock(NULL, dhost, dport);
	if (downfd < 0)
	{
		return -1;
	}

	char header[MAXBUF];
	sprintf(header, "GET %s HTTP/1.1\r\n"
					"Host: %s\r\n"
					"User-Agent: MSIE\r\n"
					"\r\n", duri, dhost);
	LOG_NORMAL_INFO("%s", header);

	int header_len = strlen(header);
	ret = send(downfd, header, header_len, 0);
	if (ret != header_len)
	{
		LOG_PERROR_INFO("send download header error.");
		close(downfd);
		return -1;
	}

	ret = read_header(downfd, header, MAXBUF-1, NULL);
	if (ret <= 0)
	{
		LOG_ERROR_INFO("read download respons error!\n");
		close(downfd);
		return -1;
	}
	header[ret] = 0;
	header_len = ret;

	LOG_NORMAL_INFO("download response: %s\n", header);

	contentLen = get_content_length(header, header_len);

	if (contentLen < 0)
	{
		LOG_WARN_INFO("can't find content length, try chunked ... \n");
		contentLen = readChunkedTextToFile(downfd, updatefile);
		if (contentLen <= 0)
		{
			close(downfd);
			return -1;
		}

	}
	else if (contentLen > 0)
	{
		contentLen = readStaticTextToFile(downfd, contentLen, updatefile);
		if (contentLen <= 0)
		{
			close(downfd);
			return -1;
		}
	}
	else
	{
		close(downfd);
		return -1;
	}

#if 1
	//У���ļ�
	memset(cal_md5, 0, sizeof(cal_md5));
	myMd5File(updatefile, 0, contentLen, NULL, 0, cal_md5);
	if (strcmp(cal_md5, plugin_md5))
	{
		LOG_ERROR_INFO("plugin md5 error! res: %s, cal: %s\n", plugin_md5, cal_md5);
		close(downfd);
		return -1;
	}
#endif

	close(downfd);
	return 1;
}

int get_version_from_txt(char *version)
{
	int fd = open(VERSION_TEXT, O_RDWR);
	if (fd <= 0)
	{
		LOG_PERROR_INFO("open %s error.", VERSION_TEXT);
		return -1;
	}
	if (read(fd, version, 32) <= 0)
	{
		sprintf(version, "GET_VERSION_ERROR");
		LOG_PERROR_INFO("read error.");
		close(fd);
		return -1;
	}
	close(fd);
	char *p = strchr(version, '\n');
	if (p)
		*p = '\0';
	p = strchr(version, '\r');
	if (p)
		*p = '\0';
	return 0;
}

int init_update_http_header(char *http_header)
{	
	char parm[1024] = {0};

	char version[33] = {0};
	get_version_from_txt(version);
	del_tab_newline(version);
	
	GET_DEV_SHM_LOCK;
	sprintf(parm, "%s%s", GET_DEV_NO, GET_WAN_MAC);
	LOG_NORMAL_INFO("- %s\n", parm);
	char md5str[33] = {0};
	CalMd5(parm, strlen(parm), md5str);
	
	char *parm_tmp = parm;
	int offset = http_add_param("devNo=", GET_DEV_NO, parm_tmp);
	parm_tmp += offset;
	offset = http_add_param("&fwv=", GET_FW_VER, parm_tmp);
	parm_tmp += offset;
	offset = http_add_param("&gccv=", GET_GCC_VER, parm_tmp);
	parm_tmp += offset;
	offset = http_add_param("&token=", md5str, parm_tmp);
	parm_tmp += offset;
	offset = http_add_param("&pv=", version, parm_tmp);
	parm_tmp += offset;
	*parm_tmp = 0;
	GET_DEV_SHM_UNLOCK;
	
	sprintf(http_header, "POST /rapi/api/plugin/update HTTP/1.1\r\n"
					 "Host: %s:%d\r\n"
					 "Content-Length: %d\r\n"
					 "Content-Type:application/x-www-form-urlencoded\r\n"
					 "\r\n"
					 "%s",
					UPDATE_HOST, UPDATE_PORT, strlen(parm), parm);
	LOG_NORMAL_INFO("%s\n", http_header);

	return strlen(http_header);
}

int extract_upate_file()
{
	remove(FILE_PATH);

	mkdir(FILE_PATH, 0777);

	//��ѹ
	char tar_cmd[255];
	EXECUTE_CMD(tar_cmd, "tar -xvf %s -C %s", UPDATE_PATH, FILE_PATH);
	
	remove(UPDATE_PATH);
	
	//ִ�а��еĽű�
	EXECUTE_CMD(tar_cmd, "chmod +x %s", SHELL_FILE);
	EXECUTE_CMD(tar_cmd, SHELL_FILE);
	
	return 1;
}

int RequestUpdate()
{
	int sockfd = open_clientfd_noblock(NULL, UPDATE_HOST, UPDATE_PORT);
	if (sockfd <= 0)
	{
		LOG_ERROR_INFO("connect update server error!\n");
		return -1;
	}
		
	/**��������*/
	char http[MAXBUF];
	int send_len = init_update_http_header(http);
 	if (socket_send(sockfd, http, send_len) != send_len)
 	{
		LOG_ERROR_INFO("socket_send update server error!\n");
		close(sockfd);
		return -1;
 	}

 	int ret = download(sockfd);
 	close(sockfd);
	if (ret < 0)
	{
		//���³���
		LOG_SAVE("update error!\n");
		return -1;
	}
	else if (ret == 0)
	{
		//û�и���
		LOG_SAVE("no new high version program file to update.\n");
		return 0;
	}
	else
	{
		return extract_upate_file();
	}

	sleep(3);
	
	return 0;
}

static bool _update_flag = false;
bool isResUpdata()
{
	return _update_flag;
}
void CleanResUpdata()
{
	_update_flag = false;
}
void SetResUpdata()
{
	_update_flag = true;
}

void *update_work(void *arg)
{
	pthread_detach(pthread_self());

	int cur_time = 0;
	int old_time = 0;
	bool update_ok = false;
	
	while (1)
	{
		cur_time = GetSystemTime_Sec();
		if (isResUpdata() || abs(cur_time - old_time) > 1 * _1_HOUR_)
		{
			//��ֵ��ǰʱ��,�����´μ���
			old_time = cur_time;

			//������
			CleanResUpdata();

			//��ʼ������£�����0��ʾ������£�1��ʾ���³ɹ���<0��ʾ���³���
			int ret = RequestUpdate();
			if (ret > 0)
			{
				//���³ɹ���׼�����������µĸ��°�
				LOG_SAVE("update success, restart program .\n");
				update_ok = true;
			}
			else if (ret == 0)
			{
				//�������,��1Сʱ���ٴθ���
			}
			else
			{
				//����ʧ��,��С��ʱ�����ٴ�ȥ����
			}			
		}

		if (update_ok)
		{
			update_ok = false;
			msg_send(NULL, 0, FRAME_MODULE_INIT, FRAME_MODULE_MONITOR, FRAME_CMD_RESTART_PROGRAM);
		}

		sleep(3);
	}
	
	return NULL;
}

int create_update_thread()
{
	pthread_t tdp;
	return pthread_create(&tdp, NULL, update_work, NULL);
}

