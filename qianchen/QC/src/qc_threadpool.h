#ifndef __QC_THREAD_POOL_H__
#define __QC_THREAD_POOL_H__

//#define _QC_THREAD_POOL_

/**����̫��һ���������һ���ܴ�1024���ļ���
����ȡֵΪ100������0��1��2�������������ܹ�����pipe���Ѿ�
ռ����203��������*/
#define POOL_MAX_THRED	100

/**������Ҫʹ�øýӿڣ�ʹ�����涨��ĺ�*/
bool qc_threadpool_create(short pool_cnt);


#define qc_threadpool_conf	{ \
	if (! qc_threadpool_create(POOL_MAX_THRED)) \
	{ \
		exit(-1); \
	} \
}

#endif
