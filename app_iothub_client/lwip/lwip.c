/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2003-2015 by Ryosuke Takeuchi
 *                     GJ Business Division RICOH COMPANY,LTD. JAPAN
 *  Copyright (C) 2017-2019 by TOPPERS PROJECT Educational Working Group.
 * 
 *  上記著作権者は，Free Software Foundation によって公表されている 
 *  GNU General Public License の Version 2 に記述されている条件か，以
 *  下の(1)～(4)の条件を満たす場合に限り，本ソフトウェア（本ソフトウェ
 *  アを改変したものを含む．以下同じ）を使用・複製・改変・再配布（以下，
 *  利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを再利用可能なバイナリコード（リロケータブルオブ
 *      ジェクトファイルやライブラリなど）の形で利用する場合には，利用
 *      に伴うドキュメント（利用者マニュアルなど）に，上記の著作権表示，
 *      この利用条件および下記の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを再利用不可能なバイナリコードの形または機器に組
 *      み込んだ形で利用する場合には，次のいずれかの条件を満たすこと．
 *    (a) 利用に伴うドキュメント（利用者マニュアルなど）に，上記の著作
 *        権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 利用の形態を，別に定める方法によって，上記著作権者に報告する
 *        こと．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者を免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者は，
 *  本ソフトウェアに関して，その適用可能性も含めて，いかなる保証も行わ
 *  ない．また，本ソフトウェアの利用により直接的または間接的に生じたい
 *  かなる損害に関しても，その責任を負わない．
 * 
 *  @(#) $Id$
 */
/*
 *  lwipシステム関数
 */
#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include <sil.h>
#include <string.h>
#include <target_syssvc.h>
#include "kernel_cfg.h"
#include "lwip.h"
#include "main.h"

#include "lwip/opt.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "arch/sys_arch.h"

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

#define NUM_LWIP_TASK  (LWIP_TASK_N - LWIP_TASK_1 + 1)
#define NUM_LOCK       (LWIP_LOCK_N - LWIP_LOCK_1 + 1)
#define NUM_SEM0       (LWIP0_SEM_N - LWIP0_SEM_1 + 1)

Inline void
svc_perror2(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd != E_OK && ercd != E_QOVR) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR2(expr)	svc_perror2(__FILE__, __LINE__, #expr, (expr))

struct sys_thread {
	const char *name;
	void       (*func)(struct sys_thread *);	/* エラーコールバック関数 */
	ID         threadno;
	void       *arg;
	ID         pri;
};

struct sys_sem {
	bool_t     act;
	int        count;
	ID         semid;
};

#define SYS_MBOX_SIZE 128

struct sys_mbox {
	int        first, last;
	void       *msgs[SYS_MBOX_SIZE];
	struct sys_sem *not_empty;
	struct sys_sem *not_full;
	struct sys_sem *mutex;
	int        wait_send;
};

static int use_task_num, lwip_count;
static struct sys_thread *lwip_thread;
static struct sys_thread Sys_Thread[NUM_LWIP_TASK];
static struct sys_sem    Sys_Lock[NUM_LOCK];
static struct sys_sem    Sys_Sem0[NUM_SEM0];

static struct sys_thread *
search_thread(void)
{
	struct sys_thread *pthread = &Sys_Thread[0];
	ID tid = 0;
	int i;

	get_tid(&tid);
	for(i = 0 ; i < NUM_LWIP_TASK ; i++, pthread++){
		if(pthread->threadno == tid)
			return pthread;
	}
	return NULL;
}

void abort(void)
{
	struct sys_thread *pthread = search_thread();
	syslog_2(LOG_ERROR, "abort [%s] thread(%d) !", pthread->name, pthread->threadno);
	slp_tsk();
}

void
sys_msleep(u32_t ms)
{
	dly_tsk(ms * 1000);
}

void
sys_memcpy(volatile void *d, const volatile void *s, int len)
{
  volatile char *src = (volatile char *)s;
  volatile char *dst = d;

  while(len > 0){
    *dst++ = *src++;
    len--;
  }
}

void
sys_memset(volatile void *d, unsigned char val, int len)
{
  volatile char *dst = d;

  while(len > 0){
    *dst++ = val;
    len--;
  }
}

u32_t
sys_now(void)
{
	SYSTIM	utime;
	SVC_PERROR(get_tim(&utime));
	return utime / 1000;
}

u32_t
sys_jiffies(void)
{
	SYSTIM	utime;
	SVC_PERROR(get_tim(&utime));
	return utime / 1000;
}

void
sys_init(void)
{
	static bool_t init = false;
	int i;

	if(!init){
		use_task_num = 0;
		lwip_count   = 0;
		lwip_thread  = NULL;
		for(i = 0 ; i < NUM_LWIP_TASK ; i++){
			Sys_Thread[i].threadno = LWIP_TASK_1+i;
		}
		for(i = 0 ; i < NUM_LOCK ; i++){
			Sys_Lock[i].semid = LWIP_LOCK_1+i;
			Sys_Lock[i].act   = false;
		}
		for(i = 0 ; i < NUM_SEM0 ; i++){
			Sys_Sem0[i].semid = LWIP0_SEM_1+i;
			Sys_Sem0[i].act   = false;
		}
		init = true;
	}
}


void
lwip_task(EXINF exinf)
{
	struct sys_thread *pthread = &Sys_Thread[(int)exinf];
	ID tid;
	get_tid(&tid);
	syslog_4(LOG_NOTICE, "lwip_task:start lwip task(%d:%d)[%s] pri(%d) !", (int)exinf, tid, pthread->name, pthread->pri);
	if(pthread->pri >= 128){
		SVC_PERROR(chg_pri(pthread->threadno, LWIP_DEFAULT_PRIORITY+5));
		dly_tsk(100 * 1000);
	}
	pthread->func(pthread->arg);
}

sys_thread_t
sys_thread_new(const char *name, lwip_thread_fn function, void *arg, int stacksize, int prio)
{
	struct sys_thread *pthread;
	if(use_task_num >= NUM_LWIP_TASK){
		syslog_1(LOG_ERROR, "sys_thread_new over task count(%d) !", use_task_num);
		slp_tsk();
	}
	wai_sem(LWIP_THREAD);
	pthread = &Sys_Thread[use_task_num++];
	pthread->name = name;
	pthread->pri  = prio;
	pthread->arg  = arg;
	pthread->func = (void (*)(struct sys_thread *))function;
	SVC_PERROR(act_tsk(pthread->threadno));
	sig_sem(LWIP_THREAD);
	return pthread;
}

err_t
sys_mbox_new(struct sys_mbox **mb, int size)
{
	struct sys_mbox *mbox;
	err_t  err;

	mbox = (struct sys_mbox *)malloc(sizeof(struct sys_mbox));
	if(mbox == NULL){
		syslog_0(LOG_ERROR, "## sys_mbox_new mem error 1 ##");
		return ERR_MEM;
	}

	mbox->first = mbox->last = 0;
	mbox->wait_send = 0;
	err = sys_sem_new(&mbox->not_empty, 0);
	if(err != E_OK){
		syslog_0(LOG_ERROR, "## sys_mbox_new sem error 1 ##");
		free(mbox);
		return err;
	}
	err = sys_sem_new(&mbox->not_full, 0);
	if(err != E_OK){
		syslog_0(LOG_ERROR, "## sys_mbox_new sem error 2 ##");
		free(mbox);
		return err;
	}
	err = sys_sem_new(&mbox->mutex, 1);
	if(err != E_OK){
		syslog_0(LOG_ERROR, "## sys_mbox_new sem error 3 ##");
		free(mbox);
		return err;
	}

	*mb = mbox;
	return ERR_OK;
}

void
sys_mbox_free(struct sys_mbox **mb)
{
	if ((mb != NULL) && (*mb != SYS_MBOX_NULL)) {
		struct sys_mbox *mbox = *mb;
//		SYS_STATS_DEC(mbox.used);
		sys_mutex_lock(&mbox->mutex);
		sys_sem_free(&mbox->not_empty);
		sys_sem_free(&mbox->not_full);
		sys_mutex_unlock(&mbox->mutex);
		sys_sem_free(&mbox->mutex);
		mbox->not_empty = mbox->not_full = mbox->mutex = NULL;
		free(mbox);
  }
}

err_t
sys_mbox_trypost(struct sys_mbox **mb, void *msg)
{
	u8_t first;
	struct sys_mbox *mbox;

	LWIP_ASSERT("invalid mbox", (mb != NULL) && (*mb != NULL));
	mbox = *mb;

	sys_arch_sem_wait(&mbox->mutex, 0);

	if((mbox->last + 1) >= (mbox->first + SYS_MBOX_SIZE)) {
 		sys_sem_signal(&mbox->mutex);
		return ERR_MEM;
	}

	mbox->msgs[mbox->last % SYS_MBOX_SIZE] = msg;

	if(mbox->last == mbox->first)
		first = 1;
	else
		first = 0;

	mbox->last++;

	if(first)
		sys_sem_signal(&mbox->not_empty);
	sys_sem_signal(&mbox->mutex);

	return ERR_OK;
}

err_t
sys_mbox_trypost_fromisr(sys_mbox_t *q, void *msg)
{
	return sys_mbox_trypost(q, msg);
}

void
sys_mbox_post(struct sys_mbox **mb, void *msg)
{
	u8_t first;
	struct sys_mbox *mbox;
	LWIP_ASSERT("invalid mbox", (mb != NULL) && (*mb != NULL));
 	mbox = *mb;

	sys_arch_sem_wait(&mbox->mutex, 0);

	syslog_2(LOG_NOTICE, "## sys_mbox_post mbox[%08x] msg[%08x] ##", mbox, msg);

	while ((mbox->last + 1) >= (mbox->first + SYS_MBOX_SIZE)) {
		mbox->wait_send++;
		sys_sem_signal(&mbox->mutex);
		sys_arch_sem_wait(&mbox->not_full, 0);
		sys_arch_sem_wait(&mbox->mutex, 0);
		mbox->wait_send--;
	}

	mbox->msgs[mbox->last % SYS_MBOX_SIZE] = msg;

	if(mbox->last == mbox->first)
		first = 1;
	else
		first = 0;

	mbox->last++;

	if(first)
		sys_sem_signal(&mbox->not_empty);

	sys_sem_signal(&mbox->mutex);
}

u32_t
sys_arch_mbox_fetch(struct sys_mbox **mb, void **msg, u32_t timeout)
{
	u32_t time_needed = 0;
	struct sys_mbox *mbox;

	if(mb == NULL && *mb == NULL){
		ID tid;
		get_tid(&tid);
		syslog_1(LOG_ERROR, "sys_arch_mbox_fetch mail box error task(%d) !", tid);
		slp_tsk();
	}
	mbox = *mb;

	/* The mutex lock is quick so we don't bother with the timeout
		stuff here. */
	sys_arch_sem_wait(&mbox->mutex, 0);

	while (mbox->first == mbox->last) {
		sys_sem_signal(&mbox->mutex);

		/* We block while waiting for a mail to arrive in the mailbox. We
		   must be prepared to timeout. */
		if(timeout != 0) {
			time_needed = sys_arch_sem_wait(&mbox->not_empty, timeout);
			if (time_needed == SYS_ARCH_TIMEOUT) {
				return SYS_ARCH_TIMEOUT;
			}
		}
		else{
			sys_arch_sem_wait(&mbox->not_empty, 0);
		}
		sys_arch_sem_wait(&mbox->mutex, 0);
	}

	if (msg != NULL)
		*msg = mbox->msgs[mbox->first % SYS_MBOX_SIZE];
	else
		syslog_1(LOG_NOTICE, "sys_mbox_fetch: mbox %8x, null msg", (void *)mbox);

	mbox->first++;

	if (mbox->wait_send) {
		sys_sem_signal(&mbox->not_full);
	}

	sys_sem_signal(&mbox->mutex);

	return time_needed;
}

u32_t
sys_arch_mbox_tryfetch(struct sys_mbox **mb, void **msg)
{
	struct sys_mbox *mbox;
	LWIP_ASSERT("invalid mbox", (mb != NULL) && (*mb != NULL));
	mbox = *mb;

	sys_arch_sem_wait(&mbox->mutex, 0);

	if(mbox->first == mbox->last){
		sys_sem_signal(&mbox->mutex);
		return SYS_MBOX_EMPTY;
	}

	if(msg != NULL)
		*msg = mbox->msgs[mbox->first % SYS_MBOX_SIZE];
	else
		syslog_1(LOG_NOTICE, "sys_mbox_tryfetch: mbox %08x, null msg", (void *)mbox);

	mbox->first++;

	if (mbox->wait_send) {
		sys_sem_signal(&mbox->not_full);
	}

	sys_sem_signal(&mbox->mutex);
	return 0;
}

err_t
sys_sem_new(struct sys_sem **sem, u8_t count)
{
	struct sys_sem *psem = NULL;
	int num = 0, i;

	if(count == 0){
		psem = &Sys_Sem0[0];
		num  = NUM_SEM0;
	}
	else if(count == 1){
		psem = &Sys_Lock[0];
		num  = NUM_LOCK;
	}
	else{
		syslog_1(LOG_ERROR, "sys_sem_new no request count(%d) !", count);
		slp_tsk();
	}
	for(i = 0 ; i < num ; i++, psem++){
		if(!psem->act){
			psem->count = count;
			psem->act = true;
			*sem = psem;
			return ERR_OK;
		}
	}
	return ERR_MEM;
}

u32_t
sys_arch_sem_wait(struct sys_sem **s, u32_t timeout)
{
	struct sys_sem *psem = *s;
	ER ercd;

	if(timeout == 0)
		ercd = wai_sem(psem->semid);
	else
		ercd = twai_sem(psem->semid, timeout * 1000);
	if(ercd == E_TMOUT)
		return SYS_ARCH_TIMEOUT;
	else if(ercd == E_OK)
		return 0;
	else{
		syslog_1(LOG_ERROR, "sys_arch_sem_wait error (%d) !", ercd);
		slp_tsk();
	}
	return 0;
}

void
sys_sem_signal(struct sys_sem **s)
{
	struct sys_sem *psem = *s;
	SVC_PERROR2(sig_sem(psem->semid));
}

void
sys_sem_free(struct sys_sem **sem)
{
	struct sys_sem *psem = *sem;
	psem->act = false;
}

sys_prot_t
sys_arch_protect(void)
{
	struct sys_thread *pthread = search_thread();

	if(lwip_thread != pthread){
		wai_sem(LWIP_PROTECT);
		lwip_thread = pthread;
		lwip_count = 1;
	}
	else
		lwip_count++;
	return (sys_prot_t)pthread;
}

void
sys_arch_unprotect(sys_prot_t pval)
{
	struct sys_thread *pthread = (struct sys_thread *)pval;

	if(lwip_thread == pthread){
		if(--lwip_count == 0){
			lwip_thread = NULL;
			sig_sem(LWIP_PROTECT);
		}
	}
}

#if PPP_SUPPORT == 0
sio_fd_t sio_open(u8_t devnum)
{
	return 0;
}

void sio_send( u8_t c, sio_status_t * siostat )
{
}

u32_t sio_read(sio_status_t * siostat, u8_t *buf, u32_t size)
{
   return 0;
}

#endif

