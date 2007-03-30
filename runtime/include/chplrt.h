#ifndef _chplrt_H_
#define _chplrt_H_

#include "chpltypes.h"

#define _timervalue struct timeval
#define _init_timer(time)
extern _timervalue* _now_timer_help(_timervalue* time);
#define _now_timer(time) (*_now_timer_help(&(time)))
extern _timervalue _default_timer; // hack as a default value
#define _new_timer() (_default_timer)
#define _seconds_timer(time) ((_real64)((time).tv_sec))
#define _microseconds_timer(time) ((_real64)((time).tv_usec))
_int32 _now_year(void);
_int32 _now_month(void);
_int32 _now_day(void);
_real64 _now_time(void);

#define array_get(x, i) (&((x)->_data[i]))
#define array_get_value(x, i) ((x)->_data[i])
#define array_set(x, i, v) ((x)->_data[i] = v)
#define array_init(x, type, size, lineno, filename) (x)->_data = _chpl_malloc(size, sizeof(type), "_data", lineno, filename)
#define array_free_elts(x, i, call) for(i = 0; i < (x)->size; i++) call
#define array_free(x, lineno, filename) _chpl_free((x)->_data, lineno, filename)

#define _noop(x)

#define malloc  dont_use_malloc_use_chpl_malloc_instead
#define calloc  dont_use_calloc_use_chpl_calloc_instead
#define free    dont_use_free_use_chpl_free_instead
#define realloc dont_use_realloc_use_chpl_realloc_instead
#define exit    dont_use_exit_use_chpl_exit_instead

#endif
