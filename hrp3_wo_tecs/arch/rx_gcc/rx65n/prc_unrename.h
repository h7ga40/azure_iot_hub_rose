/* This file is generated from prc_rename.def by genrename. */

/* This file is included only when prc_rename.h has been included. */
#ifdef TOPPERS_PRC_RENAME_H
#undef TOPPERS_PRC_RENAME_H

/*
 *  prc_kernel_impl.c
 */
#undef config_int

/*
 *  prc_timer.c
 */
#undef target_hrt_initialize
#undef target_hrt_terminate
#undef target_hrt_set_event
#undef target_hrt_raise_event
#undef target_hrt_handler
#undef target_twdtimer_initialize
#undef target_twdtimer_terminate
#undef target_twdtimer_start
#undef target_twdtimer_stop
#undef target_twdtimer_get_current
#undef target_twdtimer_handler

#include "core_unrename.h"

#endif /* TOPPERS_PRC_RENAME_H */
