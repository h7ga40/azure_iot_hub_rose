/* This file is generated from core_rename.def by genrename. */

/* This file is included only when core_rename.h has been included. */
#ifdef TOPPERS_CORE_RENAME_H
#undef TOPPERS_CORE_RENAME_H

/*
 *  core_support.src
 */
#undef call_exit_kernel
#undef dispatch
#undef exit_and_dispatch
#undef start_dispatch
#undef start_stask_r
#undef start_utask_r

/*
 *  core_kernel_impl.c
 */
#undef default_exc_handler
#undef default_int_handler
#undef intnest

/*
 *  kernel_cfg.c
 */
#undef cfg_int_table

/*
 *  kernel_mem.c
 */
#undef shared_mpu_num
#undef max_domain_mpu_num
#undef mpu_info_table


#endif /* TOPPERS_CORE_RENAME_H */
