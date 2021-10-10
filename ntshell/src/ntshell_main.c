#include <kernel.h>
#include <string.h>
#include "serial.h"
#include "core/ntshell.h"
#include "core/ntlibc.h"
#include "util/ntstdio.h"
#include "util/ntopt.h"
#include "ntshell_main.h"

struct ntshell_obj_t {
	ntshell_t ntshell;
	ntstdio_t ntstdio;
};

static cmd_table_info_t *cmd_table_info;

void ntshell_task_init(cmd_table_info_t *cti)
{
	cmd_table_info = cti;
}

int uart_read(char *buf, int cnt, void *extobj)
{
	while (serial_rea_dat(SIO_PORTID, buf, cnt) <= 0)
		dly_tsk(1000);
	return 1;
}

int uart_write(const char *buf, int cnt, void *extobj)
{
	return serial_wri_dat(SIO_PORTID, buf, cnt);
}

unsigned char stdio_xi(struct ntstdio_t *handle)
{
	char c;
	while (serial_rea_dat(SIO_PORTID, &c, 1) <= 0)
		dly_tsk(1000);
	return (unsigned char)c;
}

void stdio_xo(struct ntstdio_t *handle, unsigned char c)
{
	serial_wri_dat(SIO_PORTID, &c, 1);
}

int usrcmd_help(struct ntshell_obj_t *obj, int argc, char **argv)
{
	const cmd_table_t *p = cmd_table_info->table;
	for (int i = 0; i < cmd_table_info->count; i++) {
		ntstdio_printf(&obj->ntstdio, "%s\t:%s\n", p->cmd, p->desc);
		p++;
	}
	return 0;
}

static int usrcmd_ntopt_callback(long *args, void *extobj)
{
	struct ntshell_obj_t *obj = (struct ntshell_obj_t *)extobj;
	const cmd_table_t *p = cmd_table_info->table;
	int result = 0;
	int found = 0;

	if (*args == 0)
		return result;

	if (strcmp((const char *)args[1], "help") == 0) {
		found = 1;
		result = usrcmd_help(obj, args[0], (char **)&args[1]);
	}
	else for (int i = 0; i < cmd_table_info->count; i++) {
		if (strcmp((const char *)args[1], p->cmd) == 0) {
			found = 1;
			result = p->func(args[0], (char **)&args[1]);
			break;
		}
		p++;
	}

	if ((found == 0) && (((const char *)args[1])[0] != '\0'))
		ntstdio_printf(&obj->ntstdio, "Unknown command found. %s \n", (const char *)args[1]);

	return result;
}

int cmd_execute(const char *text, void *extobj)
{
	return ntopt_parse(text, usrcmd_ntopt_callback, extobj);
}

/*
 * 初期化
 */
static void ntshell_initialize(struct ntshell_obj_t *obj)
{
	ntstdio_init(&obj->ntstdio, NTSTDIO_OPTION_LINE_ECHO | NTSTDIO_OPTION_CANON | NTSTDIO_OPTION_LF_CRLF | NTSTDIO_OPTION_LF_CR, stdio_xi, stdio_xo);
}

static void ntshell_finalize(struct ntshell_obj_t *obj)
{
}

struct ntshell_obj_t ntshell_obj;

void ntshell_task(EXINF exinf)
{
	struct ntshell_obj_t *obj = &ntshell_obj/*(struct ntshell_obj_t *)exinf*/;

	ntshell_initialize(obj);

	ntshell_init(&obj->ntshell, uart_read, uart_write, cmd_execute, obj);
	ntshell_set_prompt(&obj->ntshell, "NTShell>");
	ntshell_execute(&obj->ntshell);

	ntshell_finalize(obj);
}

