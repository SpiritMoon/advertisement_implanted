#ifndef __AUTH_H__
#define __AUTH_H__

#define AUTH_ERROR		'0'		//����
#define AUTH_ALLOW		'1'		//��֤�ɹ�
#define AUTH_REDIRECTED 'r'		//��֤�ɹ�������ת��ָ��ҳ��
#define AUTH_SNIFF  	'2'

char auth(const char *path, const char *uri, char *portal, int *temp_accept_time);
void *_create_auth_server_(void *arg);

#endif

