#include <stdio.h>
#include <string.h>
#include "chplrt.h"
#include "chplcopygc.h"
#include "error.h"
#include "chplmem.h"

extern size_t cid2size(_int64 cid);

_memory_space *_from_space, *_to_space;

int totalRoots = 0;
void *rootlist[MAXROOTS];

void _addRoot(void* root) {
  /* Obviously this won't suffice. */
  rootlist[totalRoots++] = root;
}

void _deleteRoot(void) {
  totalRoots -= 1;
}

void _chpl_gc_copy_collect(void) {
  int i;
  _memory_space* tmp;
  for (i=0; i < totalRoots; i++) {
    if (STACK_PTR(rootlist[i])) {
      if (HEAP_AS_PTR(rootlist[i]) >= (void*)_to_space->head &&
          HEAP_AS_PTR(rootlist[i]) < (void*)_to_space->tail) {
        STACK_PTR(rootlist[i]) = HEAP_AS_PTR(rootlist[i]);
      } else {
        if (STACK_PTR(rootlist[i]) >= (void*)_from_space->head &&
            STACK_PTR(rootlist[i]) < (void*)_from_space->tail) {
          size_t size = cid2size(*(_int64*)STACK_PTR(rootlist[i]));
          memmove(_to_space->current, STACK_PTR(rootlist[i]), size);
          HEAP_AS_PTR(rootlist[i]) = (void*)_to_space->current;
          STACK_PTR(rootlist[i]) = (void*)_to_space->current;
          _to_space->current += size;
        } else {
          /* BAD - Something in the root set points at something that
             wasn't moved, but isn't in the from-space */
        }
      }
    }
  }
  _from_space->current = _from_space->head;
  tmp = _from_space;
  _from_space = _to_space;
  _to_space = tmp;
}

void* _chpl_gc_malloc(size_t number, size_t size, char* description,
                      _int32 lineno, _string filename) {
  char* current = NULL;
  size_t chunk = number * size;
  if (_from_space->current + chunk > _from_space->tail) {
    // Garbage collect, then allocate
    _chpl_gc_copy_collect();
    if (_from_space->current + chunk > _from_space->tail) {
      // out of memory.  Should probably realloc the heap.
      // For now, throw an error
      char message[1024];
      sprintf(message, "Out of memory allocating \"%s\"", description);
      _printError(message, lineno, filename);
    } else {
      current = _from_space->current;
      _from_space->current += chunk;
    }
  } else {
    current = _from_space->current;
    _from_space->current += chunk;
  }
  return (void*)current;
}

void _chpl_gc_init(size_t heapsize) {
  char *heap1, *heap2;

  // allocate the from and to spaces
  heap1 = (char*)_chpl_malloc(1, heapsize, "Heap 1", 1, "");
  heap2 = (char*)_chpl_malloc(1, heapsize, "Heap 2", 1, "");

  // allocate structs to point into the spaces
  _from_space = (_memory_space*)_chpl_malloc(1, sizeof(_memory_space),
                                             "Space pointer 1", 1, "");
  _to_space = (_memory_space*)_chpl_malloc(1, sizeof(_memory_space),
                                           "Space pointer 2", 1, "");

  // fill in the pointers
  _from_space->head = heap1;
  _from_space->tail = heap1+heapsize;
  _from_space->current = heap1;
  _to_space->head = heap2;
  _to_space->tail = heap2 + heapsize;
  _to_space->current = heap2;
}

