class C {
  var x: int = 3;
}

def bar() {
  var x: int = 2;
  def foo(c: C) {
    writeln((c, x));
    delete c;
  }
  foo(new C(x));
}

bar();
