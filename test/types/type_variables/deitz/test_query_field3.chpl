class C {
  type t;
  type tt;
  var x: t;
  var xx: tt;
}

def foo(c: C(tt=?tt, ?t)) {
  var y: t;
  var yy: tt;
  writeln((y, yy));
}

var c = new C(int, real);
writeln(c);
foo(c);
delete c;
