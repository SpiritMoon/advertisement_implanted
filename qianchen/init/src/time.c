#include "includes.h"

/*
*******************************
*	fun: 
*	arg:	
*	return:
*	time:
*	author:Seaving Cai
*******************************
*/
int GetSystemTime_Sec()
{
    return time((time_t*)NULL);
}

/*
*******************************
*	fun: 
*	arg:	
*	return:
*	time:
*	author:Seaving Cai
*******************************
*/
long GetTimestrip_us()
{
	struct timeval tCurr;
	gettimeofday(&tCurr, NULL);
	return (1000000L * tCurr.tv_sec + tCurr.tv_usec);
}

/*
*******************************
*	fun: 
*	arg:	
*	return:
*	time:
*	author:Seaving Cai
*******************************
*/
void GetSystemTime(int *Year, int *Mon, int *Data, int *Hour, int *Min)
{
	time_t now;          //ʵ����time_t�ṹ

	struct tm *timenow;  //ʵ����tm�ṹָ��

	time(&now);

    //time������ȡ���ڵ�ʱ��(���ʱ�׼ʱ��Ǳ���ʱ��)��Ȼ��ֵ��now
    timenow = localtime(&now);	

	//�ֱ��� �� �� �� ʱ  ��
	if (Year != NULL)
	{
		*Year = timenow->tm_year - 100;
	}
	if (Mon != NULL)
	{
		*Mon = timenow->tm_mon + 1; // 0 --- 11
	}
	if (Data != NULL)
	{
		*Data = timenow->tm_mday;
	}
	if (Hour != NULL)
	{
		*Hour = timenow->tm_hour;
	}
	if (Min != NULL)
	{
		*Min = timenow->tm_min;
	}

	//localtime�����Ѵ�timeȡ�õ�ʱ��now�����������е�ʱ��(���������õĵ���)
	//asctime������ʱ��ת�����ַ���ͨ��printf()�������
    //printf("Local   time   is   %s/n",asctime(timenow));
	//Tue Jun 10 14:25:14 2014
}

/*
*******************************
*	fun: 
*	arg:	
*	return:
*	time:
*	author:Seaving Cai
*******************************
*/
void GetAllSystemTime(int *Year, int *Mon, int *Data, int *Hour, int *Min, int *Sec, int *Weekday)
{
	time_t now;          //ʵ����time_t�ṹ

	struct tm *timenow;  //ʵ����tm�ṹָ��

	time(&now);

    //time������ȡ���ڵ�ʱ��(���ʱ�׼ʱ��Ǳ���ʱ��)��Ȼ��ֵ��now
    timenow = localtime(&now);

	//�ֱ��� �� �� �� ʱ  ��
	if (Year != NULL)
	{
		*Year = timenow->tm_year - 100;
	}
	if (Mon != NULL)
	{
		*Mon = timenow->tm_mon + 1; // 0 --- 11
	}
	if (Data != NULL)
	{
		*Data = timenow->tm_mday;
	}
	if (Hour != NULL)
	{
		*Hour = timenow->tm_hour;
	}
	if (Min != NULL)
	{
		*Min = timenow->tm_min;
	}
	if (Sec != NULL)
	{
		*Sec = timenow->tm_sec;
	}
	if (Weekday != NULL)
	{
		*Weekday = timenow->tm_wday;
	}
}

char * weekday(int day)
{
	switch (day)
	{
		case 0:
			return "Sun";
		case 1:
			return "Mon";
		case 2:
			return "Tue";
		case 3:
			return "Wed";
		case 4:
			return "Thu";
		case 5:
			return "Fri";
		case 6:
			return "Sat";			
	}
	
	return "unkown";
}

char * month(int month)
{
	switch (month)
	{
		case 1:
			return "Jan";
		case 2:
			return "Feb";
		case 3:
			return "Mar";
		case 4:
			return "Apr";
		case 5:
			return "May";
		case 6:
			return "Jun";
		case 7:
			return "Jul";
		case 8:
			return "Aug";
		case 9:
			return "Sep";
		case 10:
			return "Oct";
		case 11:
			return "Nov";
		case 12:
			return "Dec";
	}
	
	return "unkown";
}










