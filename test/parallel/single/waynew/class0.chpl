use Time;
// Test single field in a class

class D {
  var s: single real;
}

var d: D = D();
var f: real;

begin {
  writeln( "2: got ", d.s);
  writeln( "2: got ", d.s);
}
f = 4.0;
writeln( "1: going to sleep with ", f);
sleep( 3);
writeln( "1: woke up. writing ", f);
d.s = f;

