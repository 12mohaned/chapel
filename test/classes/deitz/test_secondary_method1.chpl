class foo {
  var i : int;
  function primary() {
    writeln("primary method call");
  }
}

function foo.secondary() {
  writeln("secondary method call");
}

var f : foo = foo();

f.primary();
f.secondary();
