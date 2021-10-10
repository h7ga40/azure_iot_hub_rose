/* This file is generated from core_rename.def by genrename. */

#ifndef TOPPERS_CORE_RENAME_H
#define TOPPERS_CORE_RENAME_H

/*
 *  core_support.src
 */
#define call_exit_kernel			_kernel_call_exit_kernel
#define dispatch					_kernel_dispatch
#define exit_and_dispatch			_kernel_exit_and_dispatch
#define start_dispatch				_kernel_start_dispatch
#define start_stask_r				_kernel_start_stask_r
#define start_utask_r				_kernel_start_utask_r

/*
 *  core_kernel_impl.c
 */
#define default_exc_handler			_kernel_default_exc_handler
#define default_int_handler			_kernel_default_int_handler
#define intnest						_kernel_intnest

/*
 *  kernel_cfg.c
 */
#define cfg_int_table				_kernel_cfg_int_table

/*
 *  kernel_mem.c
 */
#define shared_mpu_num				_kernel_shared_mpu_num
#define max_domain_mpu_num			_kernel_max_domain_mpu_num
#define mpu_info_table				_kernel_mpu_info_table


#endif /* TOPPERS_CORE_RENAME_H */
