class C {
  var a : int;
}
class D {
  var aa : int;
  fun a(i : int) { return aa + i; }
  fun =a(i : int, v : int) { aa = i + v; }
}
var d : domain(1) = [1..3];
class E {
  var a : [d] int;
}

var a : [d] int;
var x = 1;
var z = 1;
var Y1 = C();
var Y2 = D();
var Y3 = E();

a(x) = z;
writeln(a(x));
Y1.a = z;
writeln(Y1.a);
Y2.a(2) = z;
writeln(Y2.a(2));
Y3.a(2) = z;
writeln(Y3.a(2));
