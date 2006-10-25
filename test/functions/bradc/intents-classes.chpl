
class pair {
  var a: int;
  var b: real;
}

def callin(in x: pair) {
  writeln("in callin, x is: ", x.a, " ", x.b);
  x.a += 1;
  x.b += 1.1;
  writeln("re-assigned to be: ", x.a, " ", x.b);
}


def callout(out x: pair) {
  writeln("in callout, x ought to be nil");
  x = pair();
  x.a = 12;
  x.b = 4.5;
  writeln("re-assigned new instance to be: ", x.a, " ", x.b);
}


def callinout(inout x: pair) {
  writeln("in callinout, x is: ", x.a, " ", x.b);
  x.a += 1;
  x.b += 1.1;
  writeln("re-assigned to be: ", x.a, " ", x.b);
}


def callblank(x: pair) {
  writeln("in callblank, x is: ", x.a, " ", x.b);
  x.a += 1;
  x.b += 1.1;
  writeln("re-assigned to be: ", x.a, " ", x.b);
}


def main() {
  var a: pair = pair();
  a.a = 10;
  a.b = 2.3;

  callin(a);
  writeln("back at callsite, a is: ", a.a, " ", a.b);
  writeln();

  callout(a);
  writeln("back at callsite, a is: ", a.a, " ", a.b);
  writeln();

  callinout(a);
  writeln("back at callsite, a is: ", a.a, " ", a.b);
  writeln();

  callblank(a);
  writeln("back at callsite, a is: ", a.a, " ", a.b);
}
