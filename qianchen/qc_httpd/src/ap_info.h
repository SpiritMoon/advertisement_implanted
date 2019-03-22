#ifndef __AP_INFO_H__
#define __AP_INFO_H__
#if 0
typedef struct _ap_wireless_info_
{
	char *device;			//�ӿ����� 'radio0'...
	char *hwmode;			//wifiƵ��: '11a': 5G  '11g': 2.4G �ṩƵ���л������ǲ��ṩhwmode����
	char *htmode;			//Ƶ�� HT20  HT40  (iwinfo wlan0 htmodelist)
	int txpower;			//���� (iwinfo wlan0 txpowerlist)
	char *channel;			//ͨ�� (1,6,11 �����豸���ص�) (iwinfo wlan0 freqlist)
	char disabled;			//�ر����߹��� '0'��ʾ������'1'��ʾ�ر�

	char *network;			//����ģʽ��'lan'��������'wan'����
	char *mode;				//����ģʽ��'ap'��'sta'
	char *key;				//����
	char *ssid;				//ssid wifi����
	char *encryption;		//���ܷ�ʽ psk
	char hidden;			//����ssid '0'��ʾ�����أ�'1'��ʾ����
} ap_wireless_info_t;

/**������AC+APģʽ�µ�AP��Ҫ���м�
* �м̿�����Ҫnetwork�����ļ�:
* 1. config interface 'lan'�������ipaddr,netmask,ip6assingn,dns,gw�Ⱦ�̬����
* 2. config interface 'lan'����option proto 'dhcp'
* 3. config interface 'wan'�������
* 4. config interface 'wan6'�������
*/
/*typedef struct _ap_network_info_
{
	
} ap_network_info_t;
*/

/**������AC+APģʽ�µ�AP��Ҫ���м�
* �м̿�����Ҫdhcp�����ļ�:
* 1. config dhcp 'lan'�Ľ����Ҫ����
* 2. config dhcp 'wan'����޸�interface 'wan'Ϊ'lan', config dhcp 'wan'�޸�Ϊ'lan'
*/
/*typedef struct _ap_dhcp_info_
{
	
} ap_dncp_info_t;
*/

typedef struct _ap_base_info_
{
	char *ip;				//�豸�����ip
	char *mac;				//�豸mac

	char *dev_name;			//�豸���ƣ����û��Լ�����ҳ����������
	char *address;			//�豸���ŵ�ַ�����û��Լ�����ҳ������
	
} ap_base_info_t;

typedef struct _ap_
{
	int update_time;		//���һ�εĸ���ʱ��

	ap_wireless_info_t *wireless_5g;	//5GƵ��
	ap_wireless_info_t *wireless_2g;	//2.4GƵ��

	_ap_base_info_ *base_info;			//��������

} ap_t;
#endif
#endif


