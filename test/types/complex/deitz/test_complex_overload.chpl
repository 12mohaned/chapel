fun foo(x : float) {
  writeln("It's a float!");
}

fun foo(x : complex) {
  writeln("It's a complex!");
}

var y : float;

writeln(y);
foo(y);

var z : complex;

writeln(z);
foo(z);
