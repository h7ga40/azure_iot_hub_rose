#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/time.h>
#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "lwip/ip4_addr.h"
#include "lwip/inet.h"
#include "main.h"
#include "kernel_cfg.h"
#include "esp_at_socket.h"

void _exit(int status)
{
	ext_ker();
	while(1);
}

int _kill(int pid, int sig)
{
	return 0;
}

int _getpid(int n)
{
	return 1;
}

int custom_rand_generate_seed(uint8_t *output, uint32_t sz)
{
	SYSTIM now;
	int32_t i;

	get_tim(&now);
	srand(now);

	for (i = 0; i < sz; i++)
		output[i] = rand();

	return 0;
}

struct ether_addr *ether_aton_r(const char *x, struct ether_addr *p_a)
{
	struct ether_addr a;
	char *y;
	for (int ii = 0; ii < 6; ii++) {
		unsigned long int n;
		if (ii != 0) {
			if (x[0] != ':') return 0; /* bad format */
			else x++;
		}
		n = strtoul(x, &y, 16);
		x = y;
		if (n > 0xFF) return 0; /* bad byte */
		a.ether_addr_octet[ii] = n;
	}
	if (x[0] != 0) return 0; /* bad format */
	*p_a = a;
	return p_a;
}

uint8_t DecToBcd(uint8_t value)
{
	return ((value / 10) << 4) | (value % 10);
}

uint8_t BcdToDec(uint8_t value)
{
	return (10 * (value >> 4)) | (value & 0x0F);
}

#define IER_PERIB_INTB135	0x10
#define IPR_PERIB_INTB135	135

int rtc_init()
{
	// カウントソースを選択（サブクロック）
	sil_wrb_mem((uint8_t *)RTC_RCR4_ADDR,
		sil_reb_mem((uint8_t *)RTC_RCR4_ADDR) & ~(RTC_RCR4_RCKSEL_BIT));

	// サブクロック発振器の設定
	sil_wrb_mem((uint8_t *)RTC_RCR3_ADDR,
		sil_reb_mem((uint8_t *)RTC_RCR3_ADDR) | (RTC_RCR3_RTCEN_BIT));

	// カウントソースを6クロック供給
	for (int i = 0; i < 6; i++) __asm__ __volatile__ ("nop");

	// STARTビットを“0”にする
	sil_wrb_mem((uint8_t *)RTC_RCR2_ADDR,
		sil_reb_mem((uint8_t *)RTC_RCR2_ADDR) & ~(RTC_RCR2_START_BIT));

	// RCR2.STARTビットが“0”になるのを待つ
	while ((sil_reb_mem((uint8_t *)RTC_RCR2_ADDR) & RTC_RCR2_START_BIT) != 0)
		__asm__ __volatile__ ("nop");

	// カウントモードを選択
	sil_wrb_mem((uint8_t *)RTC_RCR2_ADDR,
		sil_reb_mem((uint8_t *)RTC_RCR2_ADDR) & ~(RTC_RCR2_CNTMD_BIT));

	// RTCソフトウェアリセット実行
	sil_wrb_mem((uint8_t *)RTC_RCR2_ADDR,
		sil_reb_mem((uint8_t *)RTC_RCR2_ADDR) | (RTC_RCR2_RESET_BIT));

	// RCR2.RESETビットが“0”になるのを待つ
	while ((sil_reb_mem((uint8_t *)RTC_RCR2_ADDR) & RTC_RCR2_RESET_BIT) != 0)
		__asm__ __volatile__ ("nop");

	// 桁上げ割り込みを選択型割り込みBの135番に設定
	sil_wrb_mem((uint8_t *)ICU_SLIBXR135_ADDR, 49);

    return 1;
}

int rtc_set_time(struct tm *time)
{
	// STARTビットを“0”にする
	sil_wrb_mem((uint8_t *)RTC_RCR2_ADDR,
		sil_reb_mem((uint8_t *)RTC_RCR2_ADDR) & ~(RTC_RCR2_START_BIT));

	// RCR2.STARTビットが“0”になるのを待つ
	while ((sil_reb_mem((uint8_t *)RTC_RCR2_ADDR) & RTC_RCR2_START_BIT) != 0)
		__asm__ __volatile__ ("nop");

	// RTCソフトウェアリセット実行
	sil_wrb_mem((uint8_t *)RTC_RCR2_ADDR,
		sil_reb_mem((uint8_t *)RTC_RCR2_ADDR) | (RTC_RCR2_RESET_BIT));

	// RCR2.RESETビットが“0”になるのを待つ
	while ((sil_reb_mem((uint8_t *)RTC_RCR2_ADDR) & RTC_RCR2_RESET_BIT) != 0)
		__asm__ __volatile__ ("nop");

	// カウンタ設定
	sil_wrh_mem((uint16_t *)RTC_RYRCNT_ADDR, DecToBcd(time->tm_year + 1900 - 2000));
	sil_wrb_mem((uint8_t *)RTC_RMONCNT_ADDR, DecToBcd(time->tm_mon + 1));
	sil_wrb_mem((uint8_t *)RTC_RDAYCNT_ADDR, DecToBcd(time->tm_mday));
	sil_wrb_mem((uint8_t *)RTC_RHRCNT_ADDR, DecToBcd(time->tm_hour));
	sil_wrb_mem((uint8_t *)RTC_RMINCNT_ADDR, DecToBcd(time->tm_min));
	sil_wrb_mem((uint8_t *)RTC_RSECCNT_ADDR, DecToBcd(time->tm_sec));
	sil_wrb_mem((uint8_t *)RTC_R64CNT_ADDR, 0);

	// 時計誤差補正設定（なし）

	// STARTビットを“1”にする
	sil_wrb_mem((uint8_t *)RTC_RCR2_ADDR,
		sil_reb_mem((uint8_t *)RTC_RCR2_ADDR) | (RTC_RCR2_START_BIT));

	// RCR2.STARTビットが“1”になるのを待つ
	while ((sil_reb_mem((uint8_t *)RTC_RCR2_ADDR) & RTC_RCR2_START_BIT) == 0)
		__asm__ __volatile__ ("nop");

    return 1;
}

int rtc_get_time(struct tm *time)
{
	// ICUで桁上げ割り込み要求を禁止
	sil_wrb_mem((uint8_t *)ICU_IERm_ADDR(IER_PERIB_INTB135),
		sil_reb_mem((uint8_t *)ICU_IERm_ADDR(IER_PERIB_INTB135)) & ~(1 << 4));

	// RTCで桁上げ割り込み要求を許可
	sil_wrb_mem((uint8_t *)RTC_RCR1_ADDR,
		sil_reb_mem((uint8_t *)RTC_RCR1_ADDR) | (RTC_RCR1_CIE_BIT));

	do {
		// 割り込みステータスフラグのクリア
		sil_wrb_mem((uint8_t *)ICU_IPRr_ADDR(IPR_PERIB_INTB135),
			sil_reb_mem((uint8_t *)ICU_IPRr_ADDR(IPR_PERIB_INTB135)) & ~(1 << 4));

		// カウンタ読み出し
		time->tm_year = 2000 + BcdToDec(sil_reh_mem((uint16_t *)RTC_RYRCNT_ADDR)) - 1900;
		time->tm_mon = BcdToDec(sil_reb_mem((uint8_t *)RTC_RMONCNT_ADDR)) - 1;
		time->tm_mday = BcdToDec(sil_reb_mem((uint8_t *)RTC_RDAYCNT_ADDR));
		time->tm_hour = BcdToDec(sil_reb_mem((uint8_t *)RTC_RHRCNT_ADDR));
		time->tm_min = BcdToDec(sil_reb_mem((uint8_t *)RTC_RMINCNT_ADDR));
		time->tm_sec = BcdToDec(sil_reb_mem((uint8_t *)RTC_RSECCNT_ADDR));

		// CUP割り込みに対応したIRフラグが“1”の場合、再度カウンタを読み出す
	} while ((sil_reb_mem((uint8_t *)ICU_IPRr_ADDR(IPR_PERIB_INTB135)) & (1 << 4)) != 0);

	return 1;
}

int gettimeofday (struct timeval *__restrict tp,
			  void *__restrict tzvp)
{
	struct tm timedate;
	if (!tp) return -1;
	rtc_get_time(&timedate);
	time_t time = mktime(&timedate);
	tp->tv_sec = time;
	tp->tv_usec = 0;
	return 0;
}

int write(int __fd, const void *__buf, size_t __nbyte)
{
	ER_UINT result;

	if ((__fd != 1) && (__fd != 2))
		return 0;

	result = serial_wri_dat(TASK_PORTID, (const char *)__buf, __nbyte);
	if(result < 0){
		return 0;
	}
    return result;
}

int read(int __fd, void *__buf, size_t __nbyte)
{
	ER_UINT result;

	if (__fd != 0)
		return 0;

	result = serial_rea_dat(TASK_PORTID, (char *)__buf, __nbyte);
	if(result < 0){
		return 0;
	}
    return result;
}

int close(int fd)
{
    (void)fd;
    return -1;
}

int fstat(int fd, void *pstat)
{
    (void)fd;
    (void)pstat;
    return 0;
}

off_t lseek(int fd, off_t pos, int whence)
{
    (void)fd;
    (void)pos;
    (void)whence;
    return 0;
}

int isatty(int fd)
{
    (void)fd;
    return 1;
}
