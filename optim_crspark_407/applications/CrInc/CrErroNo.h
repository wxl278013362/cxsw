/****************************************************************************
  Copyright(C)    2020,      Creality Co.Ltd.
  File name :     CrErroNo.h
  Author :        WangJiao
  Version:        1.0
  Description:    定义错误类型 
  Other:
  Mode History:
          <author>        <time>      <version>   <desc>
          wangjaio     2020-7-20     V1.0.0.1
****************************************************************************/
#ifndef _CR_ERROR_NO_H_
#define _CR_ERROR_NO_H_

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************/
#ifdef USING_OS_LIINUX
#include <errno.h>
#else 
#define EPERM        1      /* Operation not permitted */
#define ENOENT       2      /* No such file or directory */
#define ESRCH        3      /* No such process */
#define EINTR        4      /* Interrupted system call */
#define EIO          5      /* I/O error */
#define ENXIO        6      /* No such device or address */
#define E2BIG        7      /* Argument list too long */
#define ENOEXEC      8      /* Exec format error */
#define EBADF        9      /* Bad file number */
#define ECHILD      10      /* No child processes */
#define EAGAIN      11      /* Try again */
#define ENOMEM      12      /* Out of memory */
#define EACCES      13      /* Permission denied */
#define EFAULT      14      /* Bad address */
#define ENOTBLK     15      /* Block device required */
#define EBUSY       16      /* Device or resource busy */
#define EEXIST      17      /* File exists */
#define EXDEV       18      /* Cross-device link */
#define ENODEV      19      /* No such device */
#define ENOTDIR     20      /* Not a directory */
#define EISDIR      21      /* Is a directory */
#define EINVAL      22      /* Invalid argument */
#define ENFILE      23      /* File table overflow */
#define EMFILE      24      /* Too many open files */
#define ENOTTY      25      /* Not a typewriter */
#define ETXTBSY     26      /* Text file busy */
#define EFBIG       27      /* File too large */
#define ENOSPC      28      /* No space left on device */
#define ESPIPE      29      /* Illegal seek */
#define EROFS       30      /* Read-only file system */
#define EMLINK      31      /* Too many links */
#define EPIPE       32      /* Broken pipe */
#define EDOM        33      /* Math argument out of domain of func */
#define ERANGE      34      /* Math result not representable */
#endif

#define CR_EPERM       EPERM      /* Operation not permitted */
#define CR_ENOENT      ENOENT     /* No such file or directory */
#define CR_ESRCH       ESRCH      /* No such process */
#define CR_EINTR       EINTR      /* Interrupted system call */
#define CR_EIO         EIO        /* I/O error */
#define CR_ENXIO       ENXIO      /* No such device or address */
#define CR_E2BIG       E2BIG      /* Argument list too long */
#define CR_ENOEXEC     ENOEXEC    /* Exec format error */
#define CR_EBADF       EBADF      /* Bad file number */
#define CR_ECHILD      ECHILD     /* No child processes */
#define CR_EAGAIN      EAGAIN     /* Try again */
#define CR_ENOMEM      ENOMEM     /* Out of memory */
#define CR_EACCES      EACCES     /* Permission denied */
#define CR_EFAULT      EFAULT     /* Bad address */
#define CR_ENOTBLK     ENOTBLK    /* Block device required */
#define CR_EBUSY       EBUSY      /* Device or resource busy */
#define CR_EEXIST      EEXIST     /* File exists */
#define CR_EXDEV       EXDEV      /* Cross-device link */
#define CR_ENODEV      ENODEV     /* No such device */
#define CR_ENOTDIR     ENOTDIR    /* Not a directory */
#define CR_EISDIR      EISDIR     /* Is a directory */
#define CR_EINVAL      EINVAL     /* Invalid argument */
#define CR_ENFILE      ENFILE     /* File table overflow */
#define CR_EMFILE      EMFILE     /* Too many open files */
#define CR_ENOTTY      ENOTTY     /* Not a typewriter */
#define CR_ETXTBSY     ETXTBSY    /* Text file busy */
#define CR_EFBIG       EFBIG      /* File too large */
#define CR_ENOSPC      ENOSPC     /* No space left on device */
#define CR_ESPIPE      ESPIPE     /* Illegal seek */
#define CR_EROFS       EROFS      /* Read-only file system */
#define CR_EMLINK      EMLINK     /* Too many links */
#define CR_EPIPE       EPIPE      /* Broken pipe */
#define CR_EDOM        EDOM       /* Math argument out of domain of func */
#define CR_ERANGE      ERANGE     /* Math result not representable */




/****************************************************************************/
#ifdef __cplusplus
}
#endif
#endif  //ifndef _CR_ERROR_NO_H_
/****************************************************************************/