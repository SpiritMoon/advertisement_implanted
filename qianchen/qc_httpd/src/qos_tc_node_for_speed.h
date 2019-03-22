#ifndef __QOS_TC_NODE_FOR_SPEED_H__
#define __QOS_TC_NODE_FOR_SPEED_H__

typedef struct tc_class_node
{
	int class_main_id;
	int class_minor_id;

	int rate_bps;	//�ֽ�/s
	int ceil_bps;	//�ֽ�/s

	char *dev;

	int mark;
	
} class_t;


#endif


