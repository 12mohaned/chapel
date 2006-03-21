class myclass {
  var x: int;
  var y: float;
}

fun fwrite(f:file, c:myclass) {
  fwrite(f, c.x, " ", c.y);
}

var a: myclass = myclass();
var b: myclass = myclass();

writeln("a is: ", a, ", b is: ", b);

a.x = 1;
a.y = 2.3;

b.x = 4;
b.y = 5.6;

writeln("a is: ", a, ", b is: ", b);

a = b;

a.x = 7;
b.y = 8.9;

writeln("a is: ", a, ", b is: ", b);
