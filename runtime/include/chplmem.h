#ifndef _chplmem_H_
#define _chplmem_H_

#include <stddef.h>
#include "arg.h"
#include "chpltypes.h"


void initMemTable(void);
void printMemTable(_integer64 threshold);
void resetMemStat(void);
void startTrackingMem(void);
void printMemStat(void);
void printFinalMemStat(void);

void setMemmax(_integer64 value);
void setMemstat(void);
void setMemtrack(void);
void setMemthreshold(_integer64 value);
void setMemtrace(char* memLogname);

void* _chpl_malloc(size_t number, size_t size, char* description);
void* _chpl_calloc(size_t number, size_t size, char* description);
void  _chpl_free(void* ptr);
void* _chpl_realloc(void* ptr, size_t number, size_t size, char* description);
void* _chpl_alloc(size_t size, _integer64 id, char* description);
void* _chpl_malloc(size_t number, size_t size, char* description);

#define _chpl_alloc_id(_p) (((_integer64*)(_p))[-1])


#endif
