record foo { var a : int;  }
def =(a : foo, b) {
  a.a = b.a + 10;
  return a;
}
var x : foo = foo();
var y : foo = foo();
var z : foo = x;
y.a = 1;
x = y;
writeln(x.a);
writeln(z.a);
