/*
 *		ntshellタスクのヘッダファイル
 */
#ifndef _NTSHELL_MAIN_H_
#define _NTSHELL_MAIN_H_

#define SIO_PORTID 1

typedef int(*USRCMDFUNC)(int argc, char **argv);

typedef struct
{
	const char *cmd;
	const char *desc;
	USRCMDFUNC func;
} cmd_table_t;

typedef struct
{
	const cmd_table_t *table;
	int count;
} cmd_table_info_t;

/* ntshellタスク初期化 */
void ntshell_task_init(cmd_table_info_t *cmd_table_info);

/* ntshellタスク */
void ntshell_task(EXINF exinf);

#endif	/* of #ifndef _NTSHELL_MAIN_H_ */
