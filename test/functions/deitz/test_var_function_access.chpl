class foo {
  var a : int;
}

def bar(x : foo) var {
  return x;
}

var f : foo = foo(a = 12);

writeln(f);

var i : int;

writeln(i);

i = bar(f).a;

writeln(i);

bar(f).a = 2;

writeln(f);
