// Internal data structures module

param _INIT_STACK_SIZE = 8;

class _stack {
  type eltType;
  var  size: int;
  var  top: int;
  var  data: _ddata(eltType);
  
  def initialize() {
    top = 0;
    size = _INIT_STACK_SIZE;
    data = _ddata(eltType, 8);
    data.init();
  }

  def push( e: eltType) {
    if (top == size-1) {  // supersize as necessary
      size *= 2;
      var supersize = _ddata(eltType, size);
      supersize.init();
      [i in 0..(size/2)-1] supersize[i] = data[i];
      data = supersize;
    }
    data[top] = e;
    top += 1;
  }

  def pop() {
    var e: eltType;
    if top>0 then {
      top -= 1;
      e = data[top];
    } else {
      halt( "pop() on empty stack");
    }
    return e;
  }

  def empty() {
    top = 0;
  }

  def empty? {
    return top != 0;
  }

  def length {
    return top;
  }
}


def writeln( s:_stack) {
  for i in 0..(s.top)-1 do write( " ", s.data[i]);
  writeln();
}

