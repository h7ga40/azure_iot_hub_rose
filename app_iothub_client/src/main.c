/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2012 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 *  $Id$
 */

/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/* 
 *  サンプルプログラムの本体
 */

#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include <sil.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "client.h"
#include "ntshell_main.h"
#include <time.h>
#include "main.h"

#include "lwip/opt.h"

#include "lwip/init.h"

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"

#include "lwip/ip_addr.h"

#include "lwip/dns.h"
#include "lwip/dhcp.h"

#include "lwip/stats.h"

#include "lwip/tcp.h"
#include "lwip/inet_chksum.h"

#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "apps/ping/ping.h"

#include "lwip/ip_addr.h"

#include "lwip/apps/httpd.h"
#include "lwip/apps/netbiosns.h"
#include "lwip/apps/mdns.h"
#include "lwip/apps/sntp.h"
#include "lwip/apps/snmp.h"
#include "lwip/apps/snmp_mib2.h"
#include "lwip/apps/tftp_server.h"

#if LWIP_RAW
#include "lwip/icmp.h"
#include "lwip/raw.h"
#endif
#include "netif/etharp.h"
#include "lwip/inet.h"
#include "esp_at_socket.h"
#include "iodefine.h"

uint8_t mac_addr[ETHARP_HWADDR_LEN] = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc };
err_t ethernetif_init(struct netif *netif);

/*
 *  サービスコールのエラーのログ出力
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

uint32_t LWIP_RAND(void)
{
	return (uint32_t)rand();
}

#if LWIP_IPV4
/* (manual) host IP configuration */
static ip_addr_t ipaddr, netmask, gw;
#endif /* LWIP_IPV4 */

struct netif netif;

/* ping out destination cmd option */
static ip_addr_t ping_addr;

static bool_t tcpip_init_flag = false;

/* nonstatic debug cmd option, exported in lwipopts.h */
unsigned char debug_flags;

static void init_netifs(void);

void
sntp_set_system_time(u32_t sec)
{
	struct tm current_time_val = { 0 };
	time_t current_time = (time_t)sec;

	gmtime_r(&current_time, &current_time_val);
	rtc_set_time((struct tm *)&current_time_val);

	current_time = time(NULL);
	printf("%s\n", ctime(&current_time));
}

#if LWIP_MDNS_RESPONDER
static void
srv_txt(struct mdns_service *service, void *txt_userdata)
{
	err_t res;
	LWIP_UNUSED_ARG(txt_userdata);

	res = mdns_resp_add_service_txtitem(service, "path=/", 6);
	LWIP_ERROR("mdns add service txt failed\n", (res == ERR_OK), return);
}
#endif

static void
tcpip_init_done(void *arg)
{
	sys_sem_t *sem;
	sem = (sys_sem_t *)arg;

	init_netifs();

#if LWIP_IPV4
	netbiosns_set_name("toppershost");
	netbiosns_init();
#endif /* LWIP_IPV4 */

	sntp_setoperatingmode(SNTP_OPMODE_POLL);
#if LWIP_DHCP
	sntp_servermode_dhcp(1); /* get SNTP server via DHCP */
#else /* LWIP_DHCP */
#if LWIP_IPV4
	sntp_setserver(0, netif_ip_gw4(&netif));
#endif /* LWIP_IPV4 */
#endif /* LWIP_DHCP */
	sntp_setservername(0, "ntp.nict.jp");
	sntp_init();

#if LWIP_MDNS_RESPONDER
	mdns_resp_init();
	mdns_resp_add_netif(&netif, "toppershost", 3600);
	//mdns_resp_add_service(&netif, "myweb", "_http", DNSSD_PROTO_TCP, 80, 3600, srv_txt, NULL);
#endif

	sys_sem_signal(sem);
}

/*-----------------------------------------------------------------------------------*/

#if LWIP_NETIF_STATUS_CALLBACK
static void
netif_status_callback(struct netif *nif)
{
	printf("NETIF: %c%c%d is %s\n", nif->name[0], nif->name[1], nif->num,
		netif_is_up(nif) ? "UP" : "DOWN");
#if LWIP_IPV4
	printf("IPV4: Host at %s ", ip4addr_ntoa(netif_ip4_addr(nif)));
	printf("mask %s ", ip4addr_ntoa(netif_ip4_netmask(nif)));
	printf("gateway %s\n", ip4addr_ntoa(netif_ip4_gw(nif)));
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
	printf("IPV6: Host at %s\n", ip6addr_ntoa(netif_ip6_addr(nif, 0)));
#endif /* LWIP_IPV6 */
#if LWIP_NETIF_HOSTNAME
	printf("FQDN: %s\n", netif_get_hostname(nif));
#endif /* LWIP_NETIF_HOSTNAME */

#if LWIP_MDNS_RESPONDER
	if(tcpip_init_flag)
		mdns_resp_netif_settings_changed(nif);
#endif
}
#endif /* LWIP_NETIF_STATUS_CALLBACK */

static void
init_netifs(void)
{
#if LWIP_IPV4
#if LWIP_DHCP
	IP_ADDR4(&gw,      0,0,0,0);
	IP_ADDR4(&ipaddr,  0,0,0,0);
	IP_ADDR4(&netmask, 0,0,0,0);
#endif /* LWIP_DHCP */
	netif_add(&netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw), NULL, ethernetif_init, tcpip_input);
#else /* LWIP_IPV4 */
	netif_add(&netif, NULL, ethernetif_init, tcpip_input);
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
	netif_create_ip6_linklocal_address(&netif, 1);
	netif.ip6_autoconfig_enabled = 1;
#endif
#if LWIP_NETIF_STATUS_CALLBACK
	netif_set_status_callback(&netif, netif_status_callback);
#endif /* LWIP_NETIF_STATUS_CALLBACK */
	netif_set_default(&netif);
	netif_set_up(&netif);

#if LWIP_DHCP
	dhcp_start(&netif);
#endif /* LWIP_DHCP */
}

/*-----------------------------------------------------------------------------------*/
static void
main_thread(void *arg)
{
	sys_sem_t sem;
	LWIP_UNUSED_ARG(arg);

	if(sys_sem_new(&sem, 0) != ERR_OK) {
		LWIP_ASSERT("Failed to create semaphore", 0);
	}
	tcpip_init(tcpip_init_done, &sem);
	sys_sem_wait(&sem);
	syslog_0(LOG_NOTICE, "TCP/IP initialized.");

#if LWIP_SOCKET && defined(USE_PINGSEND)
	ping_init(&ping_addr);
#endif

	syslog_0(LOG_NOTICE, "Applications started.");

	tcpip_init_flag = true;
}

int rtc_init();
int wolfSSL_Debugging_ON(void);
void esp_at_task(EXINF exinf);
int usrcmd_ping(int argc, char **argv);
int usrcmd_dhcp4c(int argc, char **argv);
int usrcmd_dnsc(int argc, char **argv);
int usrcmd_ntpc(int argc, char **argv);
int set_wifi_main(int argc, char **argv);
static int_t mode_func(int argc, char **argv);
static int_t count_func(int argc, char **argv);
static int_t at_func(int argc, char **argv);

static const cmd_table_t cmdlist[] = {
	{"ping", "ping", usrcmd_ping},
//	{"dhcpc", "DHCP Client [rel|renew|info]", usrcmd_dhcp4c},
//	{"dnsc", "DNS client", usrcmd_dnsc},
//	{"ntpc", "NTP client", usrcmd_ntpc},
	{"atmode", "AT echo mode", mode_func},
	{"atcount", "AT recived count", count_func},
	{"at", "AT command", at_func},
	{"set_wifi", "Set ssid pwd", set_wifi_main},
	{"iothub", "Asure IoT Hub Client", iothub_client_main},
	{"dps_csgen", "Generate a connection string", dps_csgen_main},
	{"set_cs", "Set connection string", set_cs_main},
	{"set_proxy", "Set proxy", set_proxy_main},
	{"clear_proxy", "Set proxy", clear_proxy_main},
};
cmd_table_info_t cmd_table_info = { cmdlist, sizeof(cmdlist) / sizeof(cmdlist[0]) };

/*
 *  メインタスク
 */
void
main_task(EXINF exinf)
{
	ER_UINT	ercd;

	setenv("TZ", "JST-9", 1);
	tzset();

	SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_NOTICE), LOG_UPTO(LOG_EMERG)));
	syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);

	/*
	 *  シリアルポートの初期化
	 *
	 *  システムログタスクと同じシリアルポートを使う場合など，シリアル
	 *  ポートがオープン済みの場合にはここでE_OBJエラーになるが，支障は
	 *  ない．
	 */
	ercd = serial_opn_por(TASK_PORTID);
	if (ercd < 0 && MERCD(ercd) != E_OBJ) {
		syslog(LOG_ERROR, "%s (%d) reported by `serial_opn_por'.",
									itron_strerror(ercd), SERCD(ercd));
	}
	SVC_PERROR(serial_ctl_por(TASK_PORTID,
							(IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)));

	/* startup defaults (may be overridden by one or more opts) */
#if LWIP_IPV4
	IP_ADDR4(&gw,      192,168,  0,1);
	IP_ADDR4(&netmask, 255,255,255,0);
	IP_ADDR4(&ipaddr,  192,168,  0,2);
#endif /* LWIP_IPV4 */

	IP_SET_TYPE_VAL(ping_addr, IPADDR_TYPE_V4);
	/* use debug flags defined by debug.h */
	debug_flags = LWIP_DBG_OFF;

	rtc_init();
	sys_init();
	sys_thread_new("main_thread", main_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

	//wolfSSL_Debugging_ON();
	iothub_client_init();

	ntshell_task_init(&cmd_table_info);
	act_tsk(NTSHELL_TASK);
	esp_at_task(0);

	syslog(LOG_NOTICE, "Sample program ends.");
	slp_tsk();
	SVC_PERROR(ext_ker());
	assert(0);
}

/* 
 *  ESP-WROOM-02テストの本体
 *  シールド上のプッシュスイッチがESP-WROOM-02の
 *  リセットスイッチです．リセット後、ATコマンドを使って、
 *  ESP-WROOM-02と通信ができます。
 *  例えば、AT+GMRは、AT GMR(return)でESP-WROOM-02に送信され
 *  ESP-WROOM-02からの受信データはコンソールに表示されます．
 */

static char    aTxBuffer[TX_BUF_SIZE];
static uint8_t aRxBuffer[RX_BUF_SIZE];

int     rx_mode = MODE_DEFAULT;
queue_t rx_queue;

static int a2i(char *str)
{
	int num = 0;

	while(*str >= '0' && *str <= '9'){
		num = num * 10 + *str++ - '0';
	}
	return num;
}

/*
 *  ECHOモード設定関数
 */
static int_t mode_func(int argc, char **argv)
{
	int mode = rx_mode;

	if(argc >= 2){
		mode = a2i(argv[1]);
		if(mode < 0 || mode > MODE_ECHO_HEX)
			mode = MODE_DEFAULT;
		rx_mode = mode;
	}
	printf("AT ECHO MODE(%d)\n", mode);
	return mode;
}

/*
 *  受信カウントコマンド取得関数
 */
static int_t count_func(int argc, char **argv)
{
	printf("AT RECIVED COUNT(%d)(%d)\n", rx_queue.size, rx_queue.clen);
	return rx_queue.clen;
}

#define BASE_CMD_LEN     4

/*
 *  AT設定コマンド関数
 */
static int_t at_func(int argc, char **argv)
{
	int  i, arg_count = BASE_CMD_LEN;
	char *p = aTxBuffer;
	char *s, c;
	ER_UINT result;
	int  caps = 1;

	for(i = 1 ; i < argc ; i++){
		arg_count++;
		arg_count += strlen(argv[i]);
	}
	*p++ = 'A';
	*p++ = 'T';
	if(arg_count > BASE_CMD_LEN){
		for(i = 1 ; i < argc ; i++){
			s = argv[i];
			*p++ = '+';
			while(*s != 0){
				c = *s++;
				if(c == '"')
					caps ^= 1;
				if(caps && c >= 'a' && c <= 'z')
					c -= 0x20;
				*p++ = c;
			}
		}
	}
	*p++ = '\r';
	*p++ = '\n';
	*p++ = 0;
	result = serial_wri_dat(AT_PORTID, (const char *)aTxBuffer, arg_count);
	if(result < 0){
		syslog_1(LOG_ERROR, "AT command send error(%d) !", result);
	}
	printf("%d:%s", arg_count, aTxBuffer);
	return arg_count;
}

int esp_at_rx_handler(void *data, int len);

/*
 *  ATコマンドシリアルタスク
 */
void esp_at_task(EXINF exinf)
{
	T_SERIAL_RPOR k_rpor;
	queue_t *rxque;
	ER_UINT	ercd;
	int i, j, len;
	int dlen = 0;
	uint8_t ch;

	init_esp_at();

	SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));
	syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);

	/*
	 * キューバッファの初期化
	 */
	rxque = &rx_queue;
	rxque->size = RX_BUF_SIZE;
	rxque->clen  = 0;
	rxque->head = 0;
	rxque->tail = 0;
	rxque->pbuffer = aRxBuffer;

	/*
	 *  ESP-WROOM02用シリアルポートの初期化
	 */
	ercd = serial_opn_por(AT_PORTID);
	if (ercd < 0 && MERCD(ercd) != E_OBJ) {
		syslog(LOG_ERROR, "%s (%d) reported by `serial_opn_por'.(AT)",
									itron_strerror(ercd), SERCD(ercd));
		slp_tsk();
	}
	SVC_PERROR(serial_ctl_por(AT_PORTID, 0));

	/* P24(ESP_EN)をHighにする */
	sil_wrb_mem((void *)(&PORT2.PODR.BYTE),
					sil_reb_mem((void *)(&PORT2.PODR.BYTE)) | (1 << 4));

	while(dlen < 500000){
		serial_ref_por(AT_PORTID, &k_rpor);
		len = k_rpor.reacnt;
		if((rxque->size - rxque->clen) < len)
			len = rxque->size - rxque->clen;
		if(len > 0){
			if(rxque->head >= rxque->tail){
				if((rxque->size - rxque->head) < len)
					i = rxque->size - rxque->head;
				else
					i = len;
				j = serial_rea_dat(AT_PORTID, (char *)&rxque->pbuffer[rxque->head], i);
				rxque->head += j;
				if(rxque->head >= rxque->size)
					rxque->head -= rxque->size;
				rxque->clen += j;
				len -= j;
			}
			if(len > 0){
				j = serial_rea_dat(AT_PORTID, (char *)&rxque->pbuffer[rxque->head], len);
				rxque->head += j;
				if(rxque->head >= rxque->size)
					rxque->head -= rxque->size;
				rxque->clen += j;
				len -= j;
			}
		}
		while(rxque->clen > 0 && rx_mode != MODE_NOECHO){
			if(rx_mode == MODE_ECHO_CHAR){
				ch = rxque->pbuffer[rxque->tail];
				if(ch >= 0x7f)
					ch = '.';
				else if(ch < 0x20 && ch != '\r' && ch != '\n')
					ch = '.';
				putchar(ch);
			}
			else
				printf("%02x ", rxque->pbuffer[rxque->tail]);
			rxque->clen--;
			rxque->tail++;
			if(rxque->tail >= rxque->size)
				rxque->tail -= rxque->size;
			dlen++;
			if((dlen % 32) == 0 && rx_mode == MODE_ECHO_HEX){
				printf("\n");
				dly_tsk(50 * 1000);
			}
		}
		if (rxque->clen > 0) {
			len = rxque->clen;
			if (rxque->tail >= rxque->head) {
				if ((rxque->size - rxque->tail) < len)
					i = rxque->size - rxque->tail;
				else
					i = len;
				j = esp_at_rx_handler((char *)&rxque->pbuffer[rxque->tail], i);
				rxque->tail += j;
				if(rxque->tail >= rxque->size)
					rxque->tail -= rxque->size;
				rxque->clen -= j;
				len -= j;
			}
			if (len > 0) {
				j = esp_at_rx_handler((char *)&rxque->pbuffer[rxque->tail], len);
				rxque->tail += j;
				if(rxque->tail >= rxque->size)
					rxque->tail -= rxque->size;
				rxque->clen -= j;
				len -= j;
			}
		}
		else {
			dly_tsk(20 * 1000);
		}
	}
	syslog_0(LOG_NOTICE, "## STOP ##");
	slp_tsk();

	syslog(LOG_NOTICE, "Sample program ends.");
//	SVC_PERROR(ext_ker());
}

int usrcmd_ping(int argc, char **argv)
{
	//ping_addr

	ping_send_now();

	return 1;
}
