class C {
  var a : int;
}
class D {
  var aa : int;
  function a(i : int) { return aa + i; }
  function =a(i : int, v : int) { aa = i + v; }
}
var d : domain(1) = 1..3;
class E {
  var a : [d] int;
}

var a : [d] int;
var x = 1;
var z = 1;
var y1 = C();
var y2 = D();
var y3 = E();

a(x) = z;
writeln(a(x));
y1.a = z;
writeln(y1.a);
y1.a() = z;
writeln(y1.a());
y2.a(2) = z;
writeln(y2.a(2));
y3.a()(2) = z;
writeln(y3.a()(2));
