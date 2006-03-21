record point {
  var x : int;
  var y : int;
}

fun +(p1 : point, p2 : point) : point {
  var p3 : point = point();
  p3.x = p1.x + p2.x;
  p3.y = p1.y + p2.y;
  return p3;
}

var a : point = point();
a.x = 1;
a.y = 1;

var b : point = point();
b.x = 1;
b.y = 1;

var c : point = point();
c.x = 1;
c.y = 1;

writeln("a: (x = ", a.x, ", y = ", a.y, ")");
writeln("b: (x = ", b.x, ", y = ", b.y, ")");
writeln("c: (x = ", c.x, ", y = ", c.y, ")");

c = a + b;

writeln("c: (x = ", c.x, ", y = ", c.y, ")");
