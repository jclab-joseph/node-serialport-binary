#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

asm (".symver fcntl64, fcntl@GLIBC_2.2.5");

int __wrap_fcntl64(int fd, int cmd, ...)
{
    int result;
    va_list va;
    va_start(va, cmd);

    switch (cmd) {
      //
      // File descriptor flags
      //
      case F_GETFD: goto takes_void;
      case F_SETFD: goto takes_int;

      // File status flags
      //
      case F_GETFL: goto takes_void;
      case F_SETFL: goto takes_int;

      // File byte range locking, not held across fork() or clone()
      //
      case F_SETLK: goto takes_flock_ptr_INCOMPATIBLE;
      case F_SETLKW: goto takes_flock_ptr_INCOMPATIBLE;
      case F_GETLK: goto takes_flock_ptr_INCOMPATIBLE;

      // File byte range locking, held across fork()/clone() -- Not POSIX
      //
      case F_OFD_SETLK: goto takes_flock_ptr_INCOMPATIBLE;
      case F_OFD_SETLKW: goto takes_flock_ptr_INCOMPATIBLE;
      case F_OFD_GETLK: goto takes_flock_ptr_INCOMPATIBLE;

      // Managing I/O availability signals
      //
      case F_GETOWN: goto takes_void;
      case F_SETOWN: goto takes_int;
      case F_GETOWN_EX: goto takes_f_owner_ex_ptr;
      case F_SETOWN_EX: goto takes_f_owner_ex_ptr;
      case F_GETSIG: goto takes_void;
      case F_SETSIG: goto takes_int;

      // Notified when process tries to open or truncate file (Linux 2.4+)
      //
      case F_SETLEASE: goto takes_int;
      case F_GETLEASE: goto takes_void;

      // File and directory change notification
      //
      case F_NOTIFY: goto takes_int;

      // Changing pipe capacity (Linux 2.6.35+)
      //
      case F_SETPIPE_SZ: goto takes_int;
      case F_GETPIPE_SZ: goto takes_void;

      // File sealing (Linux 3.17+)
      //
      case F_ADD_SEALS: goto takes_int;
      case F_GET_SEALS: goto takes_void;

      // File read/write hints (Linux 4.13+)
      //
      case F_GET_RW_HINT: goto takes_uint64_t_ptr;
      case F_SET_RW_HINT: goto takes_uint64_t_ptr;
      case F_GET_FILE_RW_HINT: goto takes_uint64_t_ptr;
      case F_SET_FILE_RW_HINT: goto takes_uint64_t_ptr;

      default:
        fprintf(stderr, "fcntl64 workaround got unknown F_XXX constant");
    }

  takes_void:
    va_end(va);
    return fcntl64(fd, cmd);

  takes_int:
    result = fcntl64(fd, cmd, va_arg(va, int));
    va_end(va);
    return result;

  takes_flock_ptr_INCOMPATIBLE:
    //
    // !!! This is the breaking case: the size of the flock
    // structure changed to accommodate larger files.  If you
    // need this, you'll have to define a compatibility struct
    // with the older glibc and make your own entry point using it,
    // then call fcntl64() with it directly (bear in mind that has
    // been remapped to the old fcntl())
    // 
    fprintf(stderr, "fcntl64 hack can't use glibc flock directly");
    exit(1);

  takes_f_owner_ex_ptr:
    result = fcntl64(fd, cmd, va_arg(va, struct f_owner_ex*));
    va_end(va);
    return result;

  takes_uint64_t_ptr:
    result = fcntl64(fd, cmd, va_arg(va, uint64_t*));
    va_end(va);
    return result;
}
