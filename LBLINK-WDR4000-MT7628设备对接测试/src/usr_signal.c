#include "includes.h"


void ppool_ignore(int signal)	
{	
	;	
} 

//�����Զ�����
void signal_task_exit()
{
	signal(SIGCHLD, SIG_IGN);
}

void signal_sigpipe()
{
	signal(SIGPIPE, SIG_IGN);
}
