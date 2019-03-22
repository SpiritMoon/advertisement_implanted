
#ifndef __QC_THREADWORK_H__
#define __QC_THREADWORK_H__

/**ö�٣�����workfun������ö�ٶ����ʱ��ö�ٳ�����Ҫ��������,
��֤�ܿ�����workfun�ĺ���
ע��:ȡֵ��Χ 0-255
*/
typedef enum _work_index_
{
	E_WORK_PROCESS_REQUEST = 0,
	E_WORK_PROCESS_RESPONSE,
	E_END = 256
} work_index_t;

typedef void *(*work_fun)(void *arg);
typedef struct _work_info_
{
	work_index_t index;		//work������
	work_fun fun;			//work��ʵ��
	//void *arg;
} work_info_t;

typedef struct _work_
{
	short offset;			//work_info������
	int workinfo_size;	//work_info_t��size
	short workinfo_cnt;	//work_info_t������
	work_info_t *work_info;
} work_t;


/**��ʼ��һ���̳߳�work����*/
bool qc_threadwork_init(short work_cnt);

/**����һ��work��������*/
bool qc_threadwork_add(work_fun fun, work_index_t index);

/**�ӻ����л�ȡ��Ӧ��work��ִ��*/
bool qc_threadwork_call(work_index_t index, void *arg);

#endif

