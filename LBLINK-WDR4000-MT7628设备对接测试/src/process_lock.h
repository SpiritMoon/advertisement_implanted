#ifndef __PROCESS_LOCK_H__
#define __PROCESS_LOCK_H__

#include "includes.h"

//�Զ���������ṹ��
typedef struct s_spinlock_t
{
	pthread_spinlock_t spinlock;
	int shm_size;
	int pid_lock;
}sprocess_lock;

sprocess_lock *process_spinlock_create(int size);
int process_getlock(sprocess_lock *prolock, int pid);
int process_unlock(sprocess_lock *prolock);


#endif

