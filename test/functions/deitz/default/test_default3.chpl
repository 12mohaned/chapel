def foo(i : int = 1, j : float = 2.0, k : string = "three") {
  writeln("foo of ", i, ", ", j, ", ", k);
}

def foo(i : float = 1.0, j : string = "two", k : int = 3) {
  writeln("foo of ", i, ", ", j, ", ", k);
}

foo(4, 5.0, "six");
foo(4, 5.0);
foo(4);

foo(4.0, "five", 6);
foo(4.0, "five");
foo(4.0);
