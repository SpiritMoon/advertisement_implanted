#include "includes.h"


/**������Ҫ���˵�host,������connect����,ֱ�Ӷ���*/
filter_host_conf filterhostconfig[] = {
	{ "google.com", 0 },
	{ "connectivitycheck.gstatic.com", 0 },
	{ "goo.gl", 0 },
	{ NULL, 0 }
};
filter_host_conf * fhconfig = filterhostconfig;
/****************END***************/

/**������Ҫ���е�host,�����й��ע��,����httpͷ�����޸�*/
accept_host_conf accepthostconfig[] = {
	//{ "mongodb.sz.zazaj.com", 0 },
	{ "connect.qq.com", 0 },
	//{ "qzone.qq.com", 0 },
	//{ "v.qq.com", 0 },			//qq��Ƶ
	//{ "video.qq.com", 0 },		//qq��Ƶ
	//{ "y.qq.com", 0 },			//qq����
	//{ "music.qq.com", 0 },		//qq����
	//{ "info.gamecenter.qq.com", 0 },	//qq��Ϸ
	//{ "play.youku.com", 0 },	//�ſᲥ��
	//{ "open.show.qq.com", 0 }, //�ֻ�QQ����ľ�������
	{ "mmsns.qpic.cn", 0 },	//΢����Ƭ�鿴
	{ "qqvideo.tc.qq.com", 0 }, //΢��Ⱥ��Ƶ,qq��Ƶ���
	{ "www.urlshare.cn", 0 },	//qq�ռ����
	{ "cgi.kg.qq.com", 0 },	//qq�ռ�ȫ��K��
	{ "c.isdspeed.qq.com", 0 },
	{ "itunes.apple.com", 0 },
	{ "product.mapi.dangdang.com", 0 }, //����app����
	{ "web1.app100698537.twsapp.com", 0 }, //qq��Ϸ��������
	{ NULL, 0 }
};
accept_host_conf * ahconfig = accepthostconfig;
/****************END***************/

/**������Ҫ���е�uri,�����й��ע��,����httpͷ�����޸�*/
accept_uri_conf accepturiconfig[] = {
	{ "itunes.apple.com", 0 },	//ƻ��app���ذ�װ
	//{ "/ftn_handler?bmd5=", 0 }, //���԰汾qq���ߴ���URI�еĹؼ���
	{ NULL, 0 }
};
accept_uri_conf * auconfig = accepturiconfig;

/****************END***************/


