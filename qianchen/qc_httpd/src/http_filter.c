#include "includes.h"

#define USERNAME_LEN	17
#define PASSWORD_LEN	17

typedef struct _cookie_
{
	char sesson_name[12];
	char sesson_id[33];
	int validity;		//��
	int start;		//��
	bool flg;
	pthread_mutex_t lock;
	char username[USERNAME_LEN];
	char password[PASSWORD_LEN];
} cookie_t;

static cookie_t s_cookie = {
	.lock = PTHREAD_MUTEX_INITIALIZER
};

void s_cookie_init()
{
	memset(s_cookie.sesson_name, 0, 12);
	memset(s_cookie.sesson_id, 0, 33);
	s_cookie.start = 0;
	s_cookie.validity = 0;
	s_cookie.flg = false;
	memset(s_cookie.username, 0, USERNAME_LEN);
	memset(s_cookie.password, 0, PASSWORD_LEN);

	sprintf(s_cookie.username, "admin");
	snprintf(s_cookie.password, PASSWORD_LEN, "admin123");

	char password[32] = {0};
	if (get_passwd(password) > 0)
	{
		snprintf(s_cookie.password, PASSWORD_LEN, password);
	}
}

void s_cookie_set(char *sesson_name, char *session_id, int start, int validity, bool flg)
{
	memset(s_cookie.sesson_name, 0, 12);
	memcpy(s_cookie.sesson_name, sesson_name, strlen(sesson_name));
	memset(s_cookie.sesson_id, 0, 33);
	memcpy(s_cookie.sesson_id, session_id, strlen(session_id));
	s_cookie.start = start;
	s_cookie.validity = validity;
	s_cookie.flg = flg;
}

void s_cookie_password_set(char *password)
{
	snprintf(s_cookie.password, PASSWORD_LEN, password);
}

void create_cookie(char *cookie)
{
	char buf[64] = {0};
	struct timeval tCurr;
	gettimeofday(&tCurr, NULL);	
	sprintf(buf, "%d%d", (int)tCurr.tv_usec, rand());

	char md5[33] = {0};
	CalMd5ByCopy(buf, strlen(buf), X_AUTH_TOKEN_KEY, X_AUTH_TOKEN_KEY_LEN, md5);

	LOG_WARN_INFO("s_cookie_set\n");
	s_cookie_set("qcsessionid", md5, GetSystemTime_Sec(), SESSION_VALIDITY, true);

	sprintf(cookie, "%s=%s", "qcsessionid", md5);
}

bool cookie_check(char *cookie)
{
	bool ret = false;
	if (! cookie)
		return ret;

	if (strlen(cookie) <= 0)
		return ret;
	
	//�ж�cookie�е�sessionid
	pthread_mutex_lock(&s_cookie.lock);
	if (s_cookie.flg && GetSystemTime_Sec() - s_cookie.start > s_cookie.validity)
	{
		LOG_NORMAL_INFO("sesson timeout!\n");
		
		//seesion��ʱ
		s_cookie_init();
	}
	pthread_mutex_unlock(&s_cookie.lock);

	//cookie������Ч��
	if (s_cookie.flg)
	{
		//ȡ��cookie�е��ֶ�sesson_name��
		char *p = straddr(cookie, s_cookie.sesson_name);
		if (p)
		{
			p += strlen(s_cookie.sesson_name);
			LOG_HL_INFO("*p: %c, s_cookie.sesson_name: %s\n", *p, s_cookie.sesson_name);
			if (*p == '=')
			{
				p += 1; //���� '='

				//��ȡ32λ��ID
				int i, j = 0;
				char session_id[33] = {0};
				for (i = 0; i < strlen(p); p ++)
				{
					if (p[i] == ' '
					|| p[i] == '\r'
					|| p[i] == '\n'
					|| p[i] == ';')
					{
						break;
					}

					session_id[j ++] = p[i];
					session_id[j] = 0;
					if (j >= 33)
						break;
				}
				
				LOG_HL_INFO("CMP: (c)%s , (s)%s\n", session_id, s_cookie.sesson_id);
				
				if (strcmp(session_id, s_cookie.sesson_id) == 0)
				{
					LOG_HL_INFO("cookie check success!\n");
					
					//���¸���session����Ч��
					s_cookie.start = GetSystemTime_Sec();
					
					ret = true;
				}
				else
				{
					LOG_ERROR_INFO("cookie check failt!\n");
					s_cookie_init();
				}
			}
		}
	}

	return ret;
}
#if 0
http_filter_t login_check(char *path, char *context)
{
	//���������ҳ�棬��ֱ�����뵽cookie��֤
	if (strcmp(path, WEB_LOGIN_CHECK_PATH))
		return E_HTTP_FILTER_NORMAL;
	
	if (! context)
	{
		//û�в�������ת��login
		LOG_ERROR_INFO("login check failt!\n");
		return E_HTTP_FILTER_REDICRECT_LOGIN;
	}
	
	//ת��
	char decode_context[strlen(context)+1];
	memset(decode_context, 0, sizeof(decode_context));
	
	url_decode(context, decode_context);
	
	//��ʼ��֤�ʺź�����
	LOG_NORMAL_INFO("after url_decode: %s\n", decode_context);

	//��ȡusername��password
	char username[USERNAME_LEN] = {0};
	char password[PASSWORD_LEN] = {0};
	getRequestParma(decode_context, "username", username, USERNAME_LEN - 1);
	getRequestParma(decode_context, "password", password, PASSWORD_LEN - 1);

	//��֤
	if (strcmp(username, s_cookie.username) == 0
	&& strcmp(password, s_cookie.password) == 0)
	{
		//��֤�ɹ�,��ת��homeҳ��
		LOG_HL_INFO("login check success!\n");
		return E_HTTP_FILTER_REDICRECT_HOME;
	}
	
	//��֤ʧ��,��ת��loginҳ��
	LOG_ERROR_INFO("login check failt!\n");
	return E_HTTP_FILTER_REDICRECT_LOGIN;
}
#endif

bool passwd_check(char *password)
{
	if (strlen(password) <= 0)
		return false;
	
	if (strlen(s_cookie.password) <= 0)
	{
		snprintf(s_cookie.password, PASSWORD_LEN, "%s", password);
	}

	//��֤
	if (strcmp(password, s_cookie.password) == 0)
	{
		//��֤�ɹ�,��ת��homeҳ��
		LOG_HL_INFO("password check success!\n");
		return true;
	}
	
	//��֤ʧ��,��ת��loginҳ��
	LOG_ERROR_INFO("password check failt!\n");
	return false;
}

#if 0
http_filter_t http_filter(char *path, char *cookie, char *post_data)
{
	if (path && strcmp(path, "/") == 0)
		return E_HTTP_FILTER_REDICRECT_LOGIN;
	
	//����/htmlĿ¼�ķŹ�
	if (path && strncmp(path, "/html/", strlen("/html/")))
		return E_HTTP_FILTER_NORMAL;

	//�������������ҳ��ͷŹ�
	if (path && (strstr(path, WEB_SET_PASSWD)
	|| strstr(path, WEB_RESET_PASSWD)))
		return E_HTTP_FILTER_NORMAL;

	if (get_passwd(s_cookie.password) <= 0)
	{
		//���뵽��������ҳ��
		LOG_ERROR_INFO("no set passwd, please set password!\n");
		return E_HTTP_FILTER_REDICRECT_SET_PASSWD;
	}

	if (! cookie || ! path)
		return E_HTTP_FILTER_ERROR;

	//�����login�������󣬾�ֱ����֤�ʻ�������
	http_filter_t ret = login_check(path, post_data);
	if (ret != E_HTTP_FILTER_NORMAL)
	{
		return ret;
	}

	//loginҳ��ֱ�ӷ��ж��Ҳ���htmlĿ¼��Ҳ����
	if (strcmp(path, WEB_LOGIN_PATH)
	 && (isStrexit(path, "html") || strcmp(path, "/") == 0))
	{
		LOG_ERROR_INFO("path: %s cookie check \n", path);
		
		//��֤cookie
		if (! cookie_check(cookie))
		{
			//��֤������ת�ص�loginҳ��
			return E_HTTP_FILTER_REDICRECT_LOGIN;
		}
	}
	
	//�ж��Ƿ��˳�
	if (isStrexit(path, WEB_LOGIN_EXIT))
	{
		s_cookie_init();
		return E_HTTP_FILTER_REDICRECT_LOGIN;
	}
	
	//��֤�ɹ�����������
	return E_HTTP_FILTER_NORMAL;
}
#endif

