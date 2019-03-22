/*************************************************************************************************************************************
 ʵ�̶ֹ����ȵĻ����������ڹ����ڴ��С�ƽʱ���ó��ȿɱ�Ļ�����
*************************************************************************************************************************************/
#include "includes.h"

#define min(x, y) ((x) < (y) ? (x) : (y))
	
/**************************************************************************************
* �������ܣ���ʼ��FIFO�ṹ��
* ��ڲ�����ָ�룬��������С
* ������ֵ��FIFOָ��
* ���ú�����
***************************************************************************************/
int ft_fifo_fix_init(FT_FIFO_FIX * fifo, unsigned int dwSize)
{
    fifo->size = dwSize;
	fifo->in = fifo->out = 0;

	fifo->iLock = 0;

	return 0;
}

/************************************************************
* �������ܣ���ʼ��FIFO�ṹ��
* ��ڲ�����ָ�룬��������С
* ������ֵ��FIFOָ��
* ���ú�����
************************************************************/
void _ft_fifo_fix_clear(FT_FIFO_FIX * fifo)
{
	fifo->in = fifo->out = 0;
}

/************************************************************
* �������ܣ���Դ�����������ݿ�����FIFO,�ı�д��ַ
* ��ڲ�����FIFOָ�룬Դ������ָ�룬���ݳ���
* ������ֵ��ʵ��д������ݸ���
* ���ú�����
************************************************************/
unsigned int _ft_fifo_fix_put(FT_FIFO_FIX *fifo,
unsigned char *buffer, unsigned int len)
{
	unsigned int l,temp;
	unsigned int freesize;

	//���FIFO�Ƿ���ʣ��ռ�
	if(fifo->out == ((fifo->in + 1) % fifo->size))
		return 0;
	
	// ����FIFO��ʣ��ռ�
	if(fifo->in >= fifo->out)
		freesize = fifo->size - fifo->in + fifo->out - 1;
	else
		freesize = fifo->out - fifo->in - 1;
	
    // ȡʣ��ռ���д�볤�ȵĽ�С�ߣ���ֹ���
	len = min(len, freesize);

	// ��������
	l = min(len, fifo->size - fifo->in % fifo->size );

    // ������ǰλ����ĩβ��һ��
	memcpy(fifo->buffer + (fifo->in % fifo->size), buffer, l);

    // ����ʣ���һ��
	memcpy(fifo->buffer, buffer + l, len - l);
	
  	// ����д���ݵ�ָ��
	temp = fifo->in;
	temp = temp + len;
	temp = temp % fifo->size;
	fifo->in = temp;

	return len;
}

/************************************************************************
* �������ܣ���FIFO�н����ݶ����û�������,�ı����ַ
* ��ڲ�����FIFOָ�룬Դ������ָ�룬ƫ�Ƶ�ַ(����ڶ���ַ),��ȡ���ݳ���
* ������ֵ��ʵ�ʶ�ȡ�ĳ���
* ���ú�����
************************************************************************/
unsigned int _ft_fifo_fix_get(FT_FIFO_FIX *fifo,
unsigned char *buffer, unsigned int offset, unsigned int len)
{
	unsigned int l,temp;
	unsigned int datasize;

	// ��黺�������Ƿ�����Ч����
	if( fifo->out  == fifo->in )
		return 0;

	// ������Ч���ݵĳ���
	if(fifo->out >= fifo->in)
		datasize = fifo->size - fifo->out + fifo->in;
	else
		datasize = fifo->in - fifo->out;

    // �����µĶ���ַ
	if(offset > datasize)
		return -1;
	else
	{
		datasize = datasize - offset;
		temp = fifo->out;
		temp = temp + offset;
		temp = temp % fifo->size;
		fifo->out = temp;
	}

	// ȡ��Ҫ��ȡ�����������ݳ��ȵĽ�С��
	len = min(len, datasize);

	// �������ݵ��û�����
	l = min(len, fifo->size - (fifo->out % fifo->size));
	memcpy(buffer, fifo->buffer + (fifo->out % fifo->size), l);
	memcpy(buffer + l, fifo->buffer, len - l);

  	// ����д���ݵ�ָ��
	temp = fifo->out;
	temp = temp + len;
	temp = temp % fifo->size;
	fifo->out = temp;

	return len;
}

/***********************************************************************
* �������ܣ���FIFO�н����ݶ����û�������,<���ı����ַ>
* ��ڲ�����FIFOָ�룬Դ������ָ�룬ƫ�Ƶ�ַ(����ڶ���ַ),��ȡ���ݳ���
* ������ֵ��ʵ�ʶ�ȡ�ĳ���
* ���ú�����
***********************************************************************/
unsigned int _ft_fifo_fix_seek(FT_FIFO_FIX *fifo,
unsigned char *buffer, unsigned int offset, unsigned int len)
{
	unsigned int l,temp;
	unsigned int datasize;

	// ��黺�������Ƿ�����Ч����
	if( fifo->out  == fifo->in )
		return 0;

	// ������Ч���ݵĳ���
	if(fifo->out > fifo->in)
		datasize = fifo->size - fifo->out + fifo->in;
	else
		datasize = fifo->in - fifo->out;

	// �����µĶ���ַ
	if(offset > datasize)
		return -1;
	else
	{
		datasize = datasize - offset;
		temp = fifo->out;
		temp = temp + offset;
		temp = temp % fifo->size;
	}
	
	// ȡ��Ҫ��ȡ�����������ݳ��ȵĽ�С��
	len = min(len, datasize);

	// �������ݵ��û�����
	l = min(len, fifo->size - (temp % fifo->size));
	memcpy(buffer, fifo->buffer + (temp % fifo->size), l);
	memcpy(buffer + l, fifo->buffer, len - l);

	return len;
}
	
/************************************************************
* �������ܣ�����FIFO�Ķ���ʼλ��
* ��ڲ�����FIFOָ�룬ƫ�Ƶ�ַ(����ڶ���ַ)
* ������ֵ��ʵ�����õ�ƫ�Ƶ�ַ
* ���ú�����
************************************************************/
unsigned int _ft_fifo_fix_setoffset(FT_FIFO_FIX *fifo, unsigned int offset)
{
	unsigned int datasize;
	unsigned int l,temp;

    // ��黺�������Ƿ�����Ч����
	if( fifo->out  == fifo->in )
		return 0;

	// ������Ч���ݵĳ���
	if(fifo->out > fifo->in)
		datasize = fifo->size - fifo->out + fifo->in;
	else
		datasize = fifo->in - fifo->out;

	// �����µĶ���ַ
	l = min(offset, datasize);
	
	temp = fifo->out;
	temp = temp + l;
	temp = temp % fifo->size;
	fifo->out = temp;

	return l;
}

unsigned int _ft_fifo_fix_getlenth(FT_FIFO_FIX *fifo)
{
	unsigned int datasize;
	
	// ��黺�������Ƿ�����Ч����
	if( fifo->out  == fifo->in )
		return 0;
	
	// ������Ч���ݵĳ���
	if(fifo->out >= fifo->in)
		datasize = fifo->size - fifo->out + fifo->in;
	else
		datasize = fifo->in - fifo->out;

	return datasize;
}

// ��������ʾ��ʹ��
void _ft_fifo_fix_lock(FT_FIFO_FIX * fifo)
{
    int i = 0;

    // TraceLog(O_NULL, "ft_fifo_LOCK\r\n");

    // �����Ľ�����ʹ��FIFO���ȴ�
    while (1 == fifo->iLock)
    {
        i++;
        usleep(1);
        if (i > 10000)
        {
            LOG_ERROR_INFO("ft_fifo_LOCK ʧ��!\r\n");
            return;
        }
    }

    fifo->iLock = 1;
}

// �����������ʾ����
void _ft_fifo_fix_unlock(FT_FIFO_FIX * fifo)
{
    // TraceLog(O_NULL, "ft_fifo_UNLOCK\r\n");

    fifo->iLock = 0;
}


