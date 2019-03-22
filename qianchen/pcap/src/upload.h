#ifndef __UPLOAD_H__
#define __UPLOAD_H__

#define RE_UPLOAD_MAX_COUNT		3		//�ش��ļ�����
#define RE_UPLOAD_TIMEOUT_SEC	30		//���ϴ��ļ�ʱ����
#define UPLOAD_FILE_MAX_COUNT	10		//һ���ϴ��ļ��ĸ���

int create_upload_thread();
char *get_upload_server_ip();

time_t get_file_create_time(char *file);
int get_file_size(const char *path);
void upload_last();

char *get_sime_sn();

#endif

