function f() {
  var s : seq of int;
  var i : int;
  while i < 10 {
    s._append_in_place(i*4);
    i += 1;
  }
  return s;
}

writeln(f());

