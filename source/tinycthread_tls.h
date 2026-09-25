#ifndef _TINYCTHREAD_TLS_H_
#define _TINYCTHREAD_TLS_H_

#include "tinycthread.h"

#if defined(TINYCTHREAD_ENABLE_THREADS) && !TINYCTHREAD_ENABLE_THREADS

#define TTHREAD_TLS(type, name, ...) type name = __VA_ARGS__
#define TTHREAD_TLS_LOAD(name) (&name)

#elif defined(TTHREAD_TLS_EMULATED) && TTHREAD_TLS_EMULATED

#include <stdlib.h>
#include <string.h>

#define TTHREAD_TLS(type, name, ...) \
  tss_t name; \
  static int tthread_tls_result_##name; \
  static void tthread_tls_init_##name(void) { \
    tthread_tls_result_##name = tss_create(&name, free); \
  } \
  static type *tthread_tls_load_##name(void) { \
    static once_flag once = ONCE_FLAG_INIT; \
    type *value; \
    call_once(&once, tthread_tls_init_##name); \
    if (tthread_tls_result_##name != thrd_success) return NULL; \
    value = (type *)tss_get(name); \
    if (value == NULL) { \
      type initial = __VA_ARGS__; \
      value = (type *)malloc(sizeof(type)); \
      if (value == NULL) return NULL; \
      memcpy(value, &initial, sizeof(type)); \
      if (tss_set(name, value) != thrd_success) { \
        free(value); \
        return NULL; \
      } \
    } \
    return value; \
  }
#define TTHREAD_TLS_LOAD(name) tthread_tls_load_##name()

#else

#define TTHREAD_TLS(type, name, ...) \
  TTHREAD_THREAD_LOCAL type name = __VA_ARGS__
#define TTHREAD_TLS_LOAD(name) (&name)

#endif

#endif /* _TINYCTHREAD_TLS_H_ */
