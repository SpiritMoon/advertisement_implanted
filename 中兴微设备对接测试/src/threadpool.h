#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__	1
 
#include <pthread.h>
 
/* Ҫִ�е��������� */
typedef struct tpool_work {
	void*				(*routine)(void*);		 /* ������ */
	void				*arg;					 /* �����������Ĳ��� */
	struct tpool_work	*next;					  
}tpool_work_t;
 
typedef struct tpools {
	int 			shutdown;					 /* �̳߳��Ƿ����� */
	int 			max_thr_num;				/* ����߳��� */
	int 			working_counts;				/* ����ִ��������߳����� */
	pthread_t		*thr_id;					/* �߳�ID���� */
	tpool_work_t	*queue_head;				/* �߳����� */
	pthread_mutex_t queue_lock;					/* �̵߳��ź��� */ 				   
	pthread_cond_t	queue_ready;
	pthread_mutex_t put_lock;					/* ����/ȡ������ */
}tpool_t;
 
/*
 * @brief	  �����̳߳� 
 * @param	  max_thr_num ����߳���
 * @return	   0: �ɹ� ����: ʧ��  
 */
int tpool_create(int max_thr_num);
 
/*
 * @brief	  �����̳߳� 
 */
int tpool_destroy();
 
/*
 * @brief	  ���̳߳����������
 * @param	 routine ������ָ��
 * @param	  arg ����������
 * @return	   0: �ɹ� ����:ʧ�� 
 */
int tpool_add_work(void*(*routine)(void*), void *arg);

int display_worker_counts();

#endif


