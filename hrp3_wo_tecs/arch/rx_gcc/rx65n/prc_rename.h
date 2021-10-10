/* This file is generated from prc_rename.def by genrename. */

#ifndef TOPPERS_PRC_RENAME_H
#define TOPPERS_PRC_RENAME_H

/*
 *  prc_kernel_impl.c
 */
#define config_int					_kernel_config_int

/*
 *  prc_timer.c
 */
#define target_hrt_initialize		_kernel_target_hrt_initialize
#define target_hrt_terminate		_kernel_target_hrt_terminate
#define target_hrt_set_event		_kernel_target_hrt_set_event
#define target_hrt_raise_event		_kernel_target_hrt_raise_event
#define target_hrt_handler			_kernel_target_hrt_handler
#define target_twdtimer_initialize	_kernel_target_twdtimer_initialize
#define target_twdtimer_terminate	_kernel_target_twdtimer_terminate
#define target_twdtimer_start		_kernel_target_twdtimer_start
#define target_twdtimer_stop		_kernel_target_twdtimer_stop
#define target_twdtimer_get_current	_kernel_target_twdtimer_get_current
#define target_twdtimer_handler		_kernel_target_twdtimer_handler

#include "core_rename.h"

#endif /* TOPPERS_PRC_RENAME_H */
