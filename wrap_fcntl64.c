#include <stdarg.h>

/*
 * Starting with GLIBC_2.28, the `fcntl()` function has a new variant
 * `fcntl64()`:
 * https://gitlab.com/freedesktop-sdk/mirrors/glibc/commit/06ab719d30b01da401150068054d3b8ea93dd12f
 * (link to a mirror, glibc has no online source browser).
 *
 * See also the release notes for the 2.28 version:
 * https://www.sourceware.org/ml/libc-alpha/2018-08/msg00003.html
 *
 * This is intended to fix a bug related to large file support on architectures
 * where off_t and off64_t are not the same underlying type.
 * To remain compatible with the previous versions, the GLIBC offers a
 * compatibility symbol for these architectures. We can link the new `fcntl()`
 * and `fcntl64()` against this symbol with the help of a wrapper.
 */
int fcntl_old(int fd, int cmd, ...);
#ifdef __i386__
__asm(".symver fcntl_old,fcntl@GLIBC_2.0");
#elif defined(__amd64__)
__asm(".symver fcntl_old,fcntl@GLIBC_2.2.5");
#elif defined(__arm__)
__asm(".symver fcntl_old,fcntl@GLIBC_2.4");
#elif defined(__aarch64__)
__asm(".symver fcntl_old,fcntl@GLIBC_2.17");
#endif

int __wrap_fcntl(int fd, int cmd, ...) {
    int ret;
    va_list vargs;
    va_start(vargs, cmd);
    ret = fcntl_old(fd, cmd, va_arg(vargs, void *));
    va_end(vargs);
    return ret;
}
int __wrap_fcntl64(int fd, int cmd, ...) {
    int ret;
    va_list vargs;
    va_start(vargs, cmd);
    ret = fcntl_old(fd, cmd, va_arg(vargs, void *));
    va_end(vargs);
    return ret;
}

