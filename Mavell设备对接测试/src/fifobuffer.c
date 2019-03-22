#include "includes.h"

#define min(x, y) ((x) < (y) ? (x) : (y))
	
/*
***********************************************************
* �������ܣ���ʼ��FIFO�ṹ��
* ��ڲ�����ָ�룬��������С
* ������ֵ��FIFOָ��
* ���ú�����
***********************************************************
*/
FT_FIFO *ft_fifo_init(unsigned int size)
{
	FT_FIFO *fifo = NULL;

	fifo = malloc(sizeof(FT_FIFO));
	if (!fifo)
	{
		LOG_PERROR_INFO("Malloc failed!(fifo)");
		return NULL;
	}

	fifo->buffer = malloc(size);
	if (fifo->buffer == NULL)
	{
		LOG_PERROR_INFO("Malloc failed!(fifo->buffer)");
		free(fifo);
		return NULL;
	}
	fifo->size = size;
	fifo->in = fifo->out = 0;
	
	if(pthread_mutex_init(&fifo->lock, NULL) != 0)
	{
		LOG_PERROR_INFO("Mutex initialization failed!");
		free(fifo);
		return NULL;
	}

	return fifo;
}

FT_FIFO *ft_fifo_alloc(unsigned int size)
{
	unsigned char *buffer = NULL;
	FT_FIFO *ret = NULL;

	buffer = malloc(size);
	if (!buffer)
		return ret;

	ret = ft_fifo_init(size);
	if (!ret)
	{
		free(buffer);
	}

	return ret;
}
/*
***********************************************************
* �������ܣ��ͷ�FIFO
* ��ڲ�����FIFOָ��
* ������ֵ��
* ���ú�����
***********************************************************
*/

void ft_fifo_free(FT_FIFO *fifo)
{
	if (fifo)
	{
		if (fifo->buffer)
		{
			free(fifo->buffer);
		}
		free(fifo);
	}
}

	
/************************************************************
* �������ܣ���ʼ��FIFO�ṹ��
* ��ڲ�����ָ�룬��������С
* ������ֵ��FIFOָ��
* ���ú�����
************************************************************/
void _ft_fifo_clear(FT_FIFO *fifo)
{
	fifo->in = fifo->out = 0;
}


/*
***********************************************************
* �������ܣ���Դ�����������ݿ�����FIFO,�ı�д��ַ
* ��ڲ�����FIFOָ�룬Դ������ָ�룬���ݳ���
* ������ֵ��ʵ��д������ݸ���
* ���ú�����
***********************************************************
*/
unsigned int _ft_fifo_put(FT_FIFO *fifo,
			   unsigned char *buffer, unsigned int len)
{
	unsigned int l,temp;
	unsigned int freesize;

	//���FIFO�Ƿ���ʣ��ռ�
	if(fifo->out == ((fifo->in + 1) % fifo->size))
		return 0;
	
	//����FIFO��ʣ��ռ�
	if(fifo->in >= fifo->out)
		freesize = fifo->size - fifo->in + fifo->out - 1;
	else
		freesize = fifo->out - fifo->in - 1;
	
    //ȡʣ��ռ���д�볤�ȵĽ�С�ߣ���ֹ���
	len = min(len, freesize);

	//��������
	l = min(len, fifo->size - fifo->in % fifo->size );
	  //������ǰλ����ĩβ��һ��
	memcpy(fifo->buffer + (fifo->in % fifo->size), buffer, l);
	  //����ʣ���һ��
	memcpy(fifo->buffer, buffer + l, len - l);
	
  	//����д���ݵ�ָ��
	temp = fifo->in;
	temp = temp + len;
	temp = temp % fifo->size;
	fifo->in = temp;

	return len;
}
/*
***********************************************************
* �������ܣ���FIFO�н����ݶ����û�������,�ı����ַ
* ��ڲ�����FIFOָ�룬Դ������ָ�룬ƫ�Ƶ�ַ(����ڶ���ַ),��ȡ���ݳ���
* ������ֵ��ʵ�ʶ�ȡ�ĳ���
* ���ú�����
***********************************************************
*/
unsigned int _ft_fifo_get(FT_FIFO *fifo,
			   unsigned char *buffer, unsigned int offset, unsigned int len)
{
	unsigned int l,temp;
	unsigned int datasize;

	//��黺�������Ƿ�����Ч����
	if( fifo->out  == fifo->in )
		return 0;

	//������Ч���ݵĳ���
	if(fifo->out >= fifo->in)
		datasize = fifo->size - fifo->out + fifo->in;
	else
		datasize = fifo->in - fifo->out;

    //�����µĶ���ַ
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

	//ȡ��Ҫ��ȡ�����������ݳ��ȵĽ�С��
	len = min(len, datasize);

	//�������ݵ��û�����
	l = min(len, fifo->size - (fifo->out % fifo->size));
	memcpy(buffer, fifo->buffer + (fifo->out % fifo->size), l);
	memcpy(buffer + l, fifo->buffer, len - l);

  	//����д���ݵ�ָ��
	temp = fifo->out;
	temp = temp + len;
	temp = temp % fifo->size;
	fifo->out = temp;

	return len;
}
/*
***********************************************************
* �������ܣ���FIFO�н����ݶ����û�������,<���ı����ַ>
* ��ڲ�����FIFOָ�룬Դ������ָ�룬ƫ�Ƶ�ַ(����ڶ���ַ),��ȡ���ݳ���
* ������ֵ��ʵ�ʶ�ȡ�ĳ���
* ���ú�����
***********************************************************
*/
unsigned int _ft_fifo_seek(FT_FIFO *fifo,
			   unsigned char *buffer, unsigned int offset, unsigned int len)
{
	unsigned int l,temp;
	unsigned int datasize;

	//��黺�������Ƿ�����Ч����
	if( fifo->out  == fifo->in )
		return 0;

	//������Ч���ݵĳ���
	if(fifo->out > fifo->in)
		datasize = fifo->size - fifo->out + fifo->in;
	else
		datasize = fifo->in - fifo->out;

	//�����µĶ���ַ
	if(offset > datasize)
		return -1;
	else
	{
		datasize = datasize - offset;
		temp = fifo->out;
		temp = temp + offset;
		temp = temp % fifo->size;
	}
	
	//ȡ��Ҫ��ȡ�����������ݳ��ȵĽ�С��
	len = min(len, datasize);

	//�������ݵ��û�����
	l = min(len, fifo->size - (temp % fifo->size));
	memcpy(buffer, fifo->buffer + (temp % fifo->size), l);
	memcpy(buffer + l, fifo->buffer, len - l);

	return len;
}

/*
***********************************************************
* �������ܣ�����FIFO�Ķ���ʼλ��
* ��ڲ�����FIFOָ�룬ƫ�Ƶ�ַ(����ڶ���ַ)
* ������ֵ��ʵ�����õ�ƫ�Ƶ�ַ
* ���ú�����
***********************************************************
*/
unsigned int _ft_fifo_setoffset(FT_FIFO *fifo, unsigned int offset)
{
	unsigned int datasize;
	unsigned int l,temp;
		//��黺�������Ƿ�����Ч����
	if( fifo->out  == fifo->in )
		return 0;

	//������Ч���ݵĳ���
	if(fifo->out > fifo->in)
		datasize = fifo->size - fifo->out + fifo->in;
	else
		datasize = fifo->in - fifo->out;

	//�����µĶ���ַ
	l = min(offset, datasize);
	
	temp = fifo->out;
	temp = temp + l;
	temp = temp % fifo->size;
	fifo->out = temp;

	return l;
}

unsigned int _ft_fifo_getlenth(FT_FIFO *fifo)
{
	unsigned int datasize;
	
	//��黺�������Ƿ�����Ч����
	if( fifo->out  == fifo->in )
		return 0;
	
	//������Ч���ݵĳ���
	if(fifo->out >= fifo->in)
		datasize = fifo->size - fifo->out + fifo->in;
	else
		datasize = fifo->in - fifo->out;

	return datasize;
}


