record foo { var a : int;  }
pragma "rename _foo_eq"
def =(x : foo, b) {
  x.a = b.a + 10;
  return x;
}
var x : foo = foo();
var y : foo = foo();
var z : foo = x;
y.a = 1;
x = y;
writeln(x.a);
writeln(z.a);
