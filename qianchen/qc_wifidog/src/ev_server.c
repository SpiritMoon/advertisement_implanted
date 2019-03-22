#include "includes.h"
#if 1
struct event_base *g_evhttp_base;

static int http_redirect(const char *address, const t_http_server *http_server)
{
	struct evhttp *http;
	struct evhttp_bound_socket *handle;

	g_evhttp_base = event_base_new();
	if (! g_evhttp_base)
	{
		LOG_PERROR_INFO("event_base_new failt !\n");
		goto end_loop;
	}

	/* Create a new evhttp object to handle requests. */
	http = evhttp_new(g_evhttp_base);
	if (! http)
	{
		goto end_loop;
	}

	int *arg = calloc(sizeof(int), 1);
	if (arg)
		*arg = QC_DOG_HTTP_REDIRECT_SERVER_PORT;

	//evhttp_set_cb(http, "/", test, NULL);
	evhttp_set_gencb(http, redirect_cb, arg);

	handle = evhttp_bind_socket_with_handle(http, address, http_server->gw_http_port);
	if (!handle)
	{
		goto end_loop;
	}

	event_base_dispatch(g_evhttp_base);

end_loop:
	if (handle)
		evhttp_del_accept_socket(http, handle);

	if (http)
		evhttp_free(http);

	if (g_evhttp_base)
		event_base_free(g_evhttp_base);

	if (arg)
		free(arg);

	return -1;
}

int server_listen_port()
{
	return QC_DOG_HTTP_REDIRECT_SERVER_PORT;
}

void *_create_http_redirect_server_(void *arg)
{
	pthread_detach(pthread_self());

	t_http_server http_server;

	for ( ; ; )
	{
		http_server.gw_http_port = QC_DOG_HTTP_REDIRECT_SERVER_PORT;

		char lan_ip[33] = {0};
		get_dev_ip(lan_ip, GET_LAN_NAME);
		http_redirect(lan_ip, &http_server);

		sleep(1);
	}

	return NULL;
}

#endif

#if 0
#include <netinet/in.h>  
#include <sys/socket.h>  
#include <fcntl.h>  
  
#include <event2/event.h>  
#include <event2/buffer.h>  
#include <event2/bufferevent.h>  
  
#include <assert.h>  
#include <unistd.h>  
#include <string.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <errno.h>  
  
void do_read(evutil_socket_t fd, short events, void *arg);  
  
//struct bufferevent�ڽ�������event(read/write)�Ͷ�Ӧ�Ļ�����(struct evbuffer *input, *output)�����ṩ��Ӧ�ĺ�����������>  
//������(����ֱ�Ӳ���bufferevent)  
//���յ����ݺ��ж��ǲ�һ��һ����Ϣ�Ľ�����������־Ϊ"over"�ַ���  
void readcb(struct bufferevent *bev, void *ctx)  
{
    printf("called readcb!\n");
    struct evbuffer *input, *output;
    char *request_line;
    size_t len;

    //��ʵ����ȡ��bufferevent�е�input
    input = bufferevent_get_input(bev);

    //��ʵ����ȡ��bufferevent�е�output
    output = bufferevent_get_output(bev);

    size_t input_len = evbuffer_get_length(input);
    LOG_NORMAL_INFO("input_len: %d\n", input_len);
    
    size_t output_len = evbuffer_get_length(output);
    LOG_NORMAL_INFO("output_len: %d\n", output_len);

	for ( ; ; )
    {
    	//��evbufferǰ��ȡ��һ�У���һ���·���Ŀ��ַ����� 
		//���ַ���������һ��,EVBUFFER_EOL_CRLF��ʾ��β��һ����ѡ�Ļس�������һ�����з�  
        request_line = evbuffer_readln(input, &len, EVBUFFER_EOL_CRLF_STRICT); 
        if(NULL == request_line)
        {
            LOG_WARN_INFO("The line has not arrived yet.\n");

            //֮����Ҫ����free����Ϊ line = mm_malloc(n_to_copy+1))�������������malloc
            //free(request_line);
            break;
        }
        else
		{
            LOG_HL_INFO("%d : %s", len, request_line);

            //�����ж��ǲ���һ����Ϣ�Ľ���
            if(strcmp(request_line, "\r\n") == 0)
            {
                char *response = "HTTP/1.1 200 OK\r\nContent-length: 3\r\n\r\n ok";
                evbuffer_add(output, response, strlen(response));
                free(request_line);
                break;
            }
            
            free(request_line);
        }
    }

    size_t input_len1 = evbuffer_get_length(input);
    LOG_NORMAL_INFO("input_len1: %d\n", input_len1);
    
    size_t output_len1 = evbuffer_get_length(output);
    LOG_NORMAL_INFO("output_len1: %d\n\n", output_len1);
}

void errorcb(struct bufferevent *bev, short error, void *ctx)
{
    if (error & BEV_EVENT_EOF)
    {
        /* connection has been closed, do any clean up here */  
        printf("connection closed\n");
    }
    else if (error & BEV_EVENT_ERROR)
    {
        /* check errno to see what error occurred */
        printf("some other error\n");
    }
    else if (error & BEV_EVENT_TIMEOUT)
    {
        /* must be a timeout event handle, handle it */
        printf("Timed out\n");
    }
    
    bufferevent_free(bev);
}

void do_accept(evutil_socket_t listener, short event, void *arg)
{
    struct event_base *base = arg;
    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    int fd = accept(listener, (struct sockaddr*)&ss, &slen);
    if (fd < 0)
    {
        perror("accept");
    }
    else if (fd > FD_SETSIZE)
    {
        close(fd);
    }
    else  
    {
        struct bufferevent *bev;
        evutil_make_socket_nonblocking(fd);

        //ʹ��bufferevent_socket_new����һ��struct bufferevent *bev��������sockfd���йܸ�event_base
        ////BEV_OPT_CLOSE_ON_FREE��ʾ�ͷ�buffereventʱ�رյײ㴫��˿ڡ��⽫�رյײ��׽��֣��ͷŵײ�bufferevent�ȡ�
        bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

        //���ö�д��Ӧ�Ļص�����
        bufferevent_setcb(bev, readcb, NULL, errorcb, NULL);
  
        //���ö�д�¼�,��ʵ�ǵ�����event_add����Ӧ��д�¼������¼���������poll��
        //�����ĵ���˵�������Ӧ�¼�����Ϊtrue��bufferevent�ǲ����д���ݵ�  
        bufferevent_enable(bev, EV_READ | EV_WRITE);
    }
}

int evhttp_server_create()
{
    evutil_socket_t listener;
    struct event_base *base;
    struct event *listener_event;
    
    listener = socket_listen(NULL, 65000, 300);
    if (listener < 0)
    {
		LOG_ERROR_INFO("socket_listen failt !\n");
		return -1;
    }
    evutil_make_socket_nonblocking(listener);

    base = event_base_new();
    if (! base)
    {
    	LOG_PERROR_INFO("event_base_new error.");
    	socket_close(listener);
        return -1;
	}
	
    listener_event = event_new(base, listener, EV_READ | EV_PERSIST, do_accept, (void*)base);
	if (! listener_event)
	{
		LOG_PERROR_INFO("event_new error.");
		event_base_free(base);
		return -1;
	}

    event_add(listener_event, NULL);

    event_base_dispatch(base);

    return 0;
}

#endif
#if 0
#include <sys/queue.h>

//for http
#include <evhttp.h>

#define MYHTTPD_SIGNATURE   "myhttpd v 0.0.1"


//������̷���SIGTERM/SIGHUP/SIGINT/SIGQUIT��ʱ����ֹevent���¼�����ѭ��
void signal_handler(int sig)
{
    switch (sig)
    {
        case SIGTERM:
        case SIGHUP:
        case SIGQUIT:
        case SIGINT:
		{
            event_loopbreak();
            exit(0);
            break;
        }
        case SIGPIPE:
        {
        	break;
        }
    }
}

//����ģ��
void httpd_handler(struct evhttp_request *req, void *arg)
{
	char output[2048] = {0};
	char tmp[1024] = {0};

	//��ȡ�ͻ��������URI(ʹ��evhttp_request_uri��ֱ��req->uri)
	const char *uri;
	uri = evhttp_request_uri(req);
	sprintf(tmp, "uri=%s\n", uri);
	strcat(output, tmp);

	sprintf(tmp, "uri=%s\n", req->uri);
	strcat(output, tmp);

	//decoded uri
	char *decoded_uri;
	decoded_uri = evhttp_decode_uri(uri);
	sprintf(tmp, "decoded_uri=%s\n", decoded_uri);
	strcat(output, tmp);

	//����URI�Ĳ���(��GET�����Ĳ���)
	struct evkeyvalq params;
	evhttp_parse_query(decoded_uri, &params);
	sprintf(tmp, "q=%s\n", evhttp_find_header(&params, "q"));
	strcat(output, tmp);
	sprintf(tmp, "s=%s\n", evhttp_find_header(&params, "s"));
	strcat(output, tmp);
	free(decoded_uri);

	//��ȡPOST����������
	char *post_data = (char *) EVBUFFER_DATA(req->input_buffer);
	sprintf(tmp, "post_data=%s\n", post_data);
	strcat(output, tmp);

	/*
	����ģ����Ը���GET/POST�Ĳ���ִ����Ӧ������Ȼ�󽫽�����
	...
	*/

	/* ������ͻ��� */

	//HTTP header
	evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
	evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
	evhttp_add_header(req->output_headers, "Connection", "close");
	
	//���������
	struct evbuffer *buf;
	buf = evbuffer_new();
	evbuffer_add_printf(buf, "It works!\n%s\n", output);
	evhttp_send_reply(req, HTTP_OK, "OK", buf);
	evbuffer_free(buf);
}

void test(struct evhttp_request *req, void *arg)
{
	httpd_handler(req, arg);
}

int evhttp_server_create()
{
    //�Զ����źŴ�����
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGPIPE, signal_handler);

    //Ĭ�ϲ���
    char *httpd_option_listen = "0.0.0.0";
    int httpd_option_port = 8080;
    int httpd_option_timeout = 120; //in seconds

    /* ʹ��libevent����HTTP Server */

    //��ʼ��event API
    event_init();

    //����һ��http server
    struct evhttp *httpd;
    httpd = evhttp_start(httpd_option_listen, httpd_option_port);
    evhttp_set_timeout(httpd, httpd_option_timeout);

    //ָ��generic callback
    //evhttp_set_gencb(httpd, httpd_handler, NULL);
    
    //Ҳ����Ϊ�ض���URIָ��callback
    evhttp_set_cb(httpd, "/", test, NULL);

    //ѭ������events
    event_dispatch();

    evhttp_free(httpd);
    
    return 0;
}

#endif

