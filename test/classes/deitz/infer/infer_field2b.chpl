// infer return type of iterator with state transform

iterator bar() : int {
  var i = 1;
  while i < 5 {
    yield i;
    i += 1;
  }
}

class C {
  var jump : int = 0;
  var result : int;
  var i : int;
}

def next_foo(c : C) : C {
  if c.jump == 0 then
    goto _0;
  else if c.jump == 1 then
    goto _1;
label _0
  c.i = 1;
  while c.i < 5 {
    c.result = c.i;
    c.jump = 1;
    return c;
label _1
    c.i += 1;
  }
  return nil;
}

def foo() {
  var c = C();
  c = next_foo(c);
  var s : list of c.result;
  while c != nil {
    s.append(c.result);
    c = next_foo(c);
  }
  return s;
}

writeln( bar());
writeln(foo());

for i in bar() do
  writeln(i);

var c = C();
c = next_foo(c);
while c != nil {
  writeln(c.result);
  c = next_foo(c);
}
