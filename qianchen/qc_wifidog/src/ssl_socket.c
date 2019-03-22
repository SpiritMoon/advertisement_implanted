#include "includes.h"

static void *my_zeroing_malloc(size_t howmuch)
{
	return calloc(1, howmuch);
}

void common_setup (void)
{
	CRYPTO_set_mem_functions(my_zeroing_malloc, realloc, free);
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();

	LOG_NORMAL_INFO("Using OpenSSL version \"%s\"\nand libevent version \"%s\"\n",
 			SSLeay_version(SSLEAY_VERSION), event_get_version());
}

#if 0
SSL_CTX *ssl_init()
{
	ERR_load_BIO_strings();
	
    /* SSL ���ʼ�� */
    SSL_library_init();
    
    /* �������� SSL �㷨 */
    OpenSSL_add_all_algorithms();
    
    /* �������� SSL ������Ϣ */
    SSL_load_error_strings();
    
    /* �� SSL V2 �� V3 ��׼���ݷ�ʽ����һ�� SSL_CTX ���� SSL Content Text */
    SSL_CTX *ctx = SSL_CTX_new(SSLv23_server_method());
    
    /* Ҳ������ SSLv2_server_method() �� SSLv3_server_method() ������ʾ V2 �� V3��׼ */
    if (ctx == NULL)
    {
        ERR_print_errors_fp(stdout);
        return NULL;
    }
    
    /* �����û�������֤�飬 ��֤���������͸��ͻ��ˡ� ֤��������й�Կ */
    if (SSL_CTX_use_certificate_file(ctx, CA_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stdout);
        ssl_ctx_free(ctx);
        return NULL;
    }
    
    /* �����û�˽Կ */
    if (SSL_CTX_use_PrivateKey_file(ctx, PRIVATE_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stdout);
        ssl_ctx_free(ctx);
        return NULL;
    }
    
    /* ����û�˽Կ�Ƿ���ȷ */
    if (! SSL_CTX_check_private_key(ctx))
    {
        ERR_print_errors_fp(stdout);
        ssl_ctx_free(ctx);
        return NULL;
    }
    
	return ctx;
}
#endif

void ssl_close(SSL *ssl)
{
	if (ssl)
	{
		/* �ر� SSL ���� */
		SSL_shutdown(ssl);
		
		/* �ͷ� SSL */
		SSL_free(ssl);

		ssl = NULL;
	}
}

void ssl_ctx_free(SSL_CTX *ctx)
{
    /* �ͷ� CTX */
    if (ctx)
    {
    	SSL_CTX_free(ctx);
	}
	
	ctx = NULL;
}

int ssl_socket_read(SSL *ssl, char *buf, int readlen, unsigned char time_out)
{
    int count = 0;
    int ret = -1;
    unsigned int last_time = SYSTEM_SEC;
    
	for ( ; ; )
    {    	
		if (time_out > 0 && SYSTEM_SEC - last_time > time_out)
		{
			return -1;
		}
		
		count = SSL_read(ssl, buf, readlen);
		ret = SSL_get_error(ssl, count);
		if (ret == SSL_ERROR_NONE)
		{
			if (count > 0)
			{
				//printf("count = %d\n", count);
				//���¸�ֵ��ʼ����ʱ��
				last_time = SYSTEM_SEC;
				return count;
			}
		}
		else if (ret == SSL_ERROR_WANT_READ)
		{
			usleep(100);
			continue;
		}
		else
		{
			LOG_ERROR_INFO("ssl read error!\n");
			return -1;
		}
    }

    return -1;
}

int ssl_socket_send(SSL *ssl, char *buf, int len, unsigned char time_out)
{
    int count = 0;
    int ret = -1;
    int offset = 0;
    int send_len = len;
    unsigned int last_time = SYSTEM_SEC;
    
    while (send_len > 0)
    {
		if (time_out > 0 && SYSTEM_SEC - last_time > time_out)
		{
			return -1;
		}
		
		count = SSL_write(ssl, buf + offset, send_len);
		ret = SSL_get_error(ssl, count);
		if (ret == SSL_ERROR_NONE)
		{
			if (count > 0)
			{
				offset += count;
				send_len -= count;
				last_time = SYSTEM_SEC;
			}
		}
		else if (ret == SSL_ERROR_WANT_READ /*SSL_ERROR_WANT_WRITE*/)
		{
			usleep(100);
			continue;
		}
		else
		{
			LOG_ERROR_INFO("ssl send error!\n");
			return -1;
		}
    }

	if (send_len == 0)
		return send_len;
	
    return -1;
}

SSL *ssl_accept(SSL_CTX *ctx, int fd, int time_out)
{
	SSL *ssl = NULL;

	/* ���� ctx ����һ���µ� SSL */
	ssl = SSL_new(ctx);
	if (! ssl)
	{
		LOG_ERROR_INFO("SSL_new failt!\n");
		return NULL;
	}
	
	/* �������û��� socket ���뵽 SSL */
	SSL_set_fd(ssl, fd);
	
	unsigned int last_time = SYSTEM_SEC;
	bool isContinue = true;
	while (isContinue)
	{
		isContinue = false;
		
		if (time_out > 0 && SYSTEM_SEC - last_time > time_out)
		{
			ssl_close(ssl);
			return NULL;
		}

		/* ����SSL���� */
		int ret = SSL_accept(ssl);
		if (ret != 1)
		{
			ret = SSL_get_error(ssl, -1);
			if (ret == SSL_ERROR_WANT_READ)
			{
				isContinue = true;
			}
			else
			{
				LOG_PERROR_INFO("--- SSL accept failt! ");
				ssl_close(ssl);
				return NULL;
			}	
		}
		else
		{
			break;
		}
	}

	return ssl;
}

