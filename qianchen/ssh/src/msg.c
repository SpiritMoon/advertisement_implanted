
#include "includes.h"

#define MSG_BLOCK	0
#define MSG_NOBLOCK	IPC_NOWAIT

// ��Ϣ
typedef struct {
	long mtype;
	char mtext[MAX_MESSAGE_LEN + 1];
} msg_info;

/**************************************************************************************************
* �������ܣ�������Ϣ����
* ��ڲ������ؼ���
* ������ֵ����Ϣ��ʶ��
* ���ú�����
**************************************************************************************************/
int CreatMsgQueue(key_t key)
{   
    int msqid;

    msqid = msgget(key, (IPC_CREAT | 0660));
    if (msqid == -1)
    {
        printf("cannot create message queue resource\r\n");
        return -1;
    }

	return msqid;
}

/**************************************************************************************************
* �������ܣ�������Ϣ
* ��ڲ�������Ϣ��ʶ������Ϣ����
* ������ֵ��
* ���ú�����
**************************************************************************************************/
int SendMsg(int msqid,char* msg, int iLen)
{
	int iResult;
	msg_info buf;
	
	buf.mtype = 1;
	memcpy(buf.mtext, msg, iLen);

	iResult = msgsnd(msqid, &buf, iLen, MSG_NOBLOCK);
	if (iResult == -1)
	{
		 printf("cannot send message to the message queue\r\n");
	}

	return iResult;
}

/**************************************************************************************************
* �������ܣ�������Ϣ
* ��ڲ�������Ϣ��ʶ������Ϣ����
* ������ֵ��
* ���ú�����
* 0x24 src dst len data 0x0d 0x0a
**************************************************************************************************/
int RecvMsg(int iQid, char* pcMsg, int len)
{
	int iResult = 0;
	int iMsgType = 0;

	if (len > MAX_MESSAGE_LEN)
		return 0;

	msg_info buffer;
	
	iResult = msgrcv(iQid, &buffer, len, iMsgType, MSG_BLOCK);
	if (iResult == -1)
	{
		return -1;
	}
	else
	{
		memcpy(pcMsg, buffer.mtext, iResult);
		pcMsg[iResult] = 0;
	}

	return iResult;
}

//--------------------------------------------------------------------------------------
// ��Ϣ�������
#define KEY_MSG_SSH     	0x10
#define KEY_MSG_INIT  		0x20
#define KEY_MSG_QCHTTPD     0x30
#define KEY_MSG_MONITOR     0x40
#define KEY_MSG_QCDOG     	0x50
#define KEY_MSG_MITM     	0x60
#define KEY_MSG_FW_SET     	0x70
#define KEY_MSG_APCTRL     	0x80

static int _InitMsgQid = -1;
static int _SshMsgQid = -1;
static int _QchttpdMsgQid = -1;
static int _MonitorMsgQid = -1;
static int _QcdogMsgQid = -1;
static int _MitmMsgQid = -1;
static int _FwSetMsgQid = -1;
static int _ApctrlMsgQid = -1;

//--------------------------------------------------------------------------------------

void InitMsgQueue(void)
{
	_SshMsgQid = CreatMsgQueue(KEY_MSG_SSH);
	_InitMsgQid = CreatMsgQueue(KEY_MSG_INIT);
	_QchttpdMsgQid = CreatMsgQueue(KEY_MSG_QCHTTPD);
	_MonitorMsgQid = CreatMsgQueue(KEY_MSG_MONITOR);
	_QcdogMsgQid = CreatMsgQueue(KEY_MSG_QCDOG);
	_MitmMsgQid = CreatMsgQueue(KEY_MSG_MITM);
	_FwSetMsgQid = CreatMsgQueue(KEY_MSG_FW_SET);
	_ApctrlMsgQid = CreatMsgQueue(KEY_MSG_APCTRL);
}

int SendMsgToSsh(char * pcMsg, int iLen)
{
    return SendMsg(_SshMsgQid, pcMsg, iLen);
}

int SendMsgToInit(char * pcMsg, int iLen)
{
    return SendMsg(_InitMsgQid, pcMsg, iLen);
}

int SendMsgToQchttpd(char * pcMsg, int iLen)
{
    return SendMsg(_QchttpdMsgQid, pcMsg, iLen);
}

int SendMsgToMonitor(char * pcMsg, int iLen)
{
    return SendMsg(_MonitorMsgQid, pcMsg, iLen);
}

int SendMsgToQcdog(char * pcMsg, int iLen)
{
    return SendMsg(_QcdogMsgQid, pcMsg, iLen);
}

int SendMsgToMitm(char * pcMsg, int iLen)
{
    return SendMsg(_MitmMsgQid, pcMsg, iLen);
}

int SendMsgToFwSet(char * pcMsg, int iLen)
{
    return SendMsg(_FwSetMsgQid, pcMsg, iLen);
}

int SendMsgToApctrl(char * pcMsg, int iLen)
{
    return SendMsg(_ApctrlMsgQid, pcMsg, iLen);
}

int RecvSshMsg(char* pcMsg)
{
	return RecvMsg(_SshMsgQid, pcMsg, MAX_MESSAGE_LEN);
}

