#include "includes.h"

//������������
void fw_vir_netdev_create()
{
	char cmd[255];
	EXECUTE_CMD(cmd, "ip link set %s up", VIR_NET_DEV);
}

//������������
void fw_vir_netdev_destroy()
{
	char cmd[255];
	EXECUTE_CMD(cmd, "ip link set %s down", VIR_NET_DEV);
}

//�ض���������������������
void fw_tc_redirect_to_vir_netdev()
{
	char cmd[255];

	//��WAN�豸�Ͻ���һ�����и����
	EXECUTE_CMD(cmd, "tc qdisc add dev %s handle ffff: ingress", LAN_DEV);
	
	//�����е������ض�����������
	EXECUTE_CMD(cmd, "tc filter add dev %s parent ffff: protocol ip u32 match u32 0 0 action mirred egress redirect dev %s", LAN_DEV, VIR_NET_DEV);
}

//ɾ���ض�����й���
void fw_tc_del_redirect_to_vir_netdev_rule()
{
	char cmd[255];
	EXECUTE_CMD(cmd, "tc qdisc del dev %s handle ffff: ingress", LAN_DEV);
}

