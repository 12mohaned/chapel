config const n: int = 4096;

def foo(i: int) {
  if i < n {
    _debugWriteln(here.id, " pre ", i);
    sync begin foo(i+1);
    _debugWriteln(here.id, " post ", i);
  }
}

foo(1);
