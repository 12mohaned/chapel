class foo {
  var x;
  fun print() {
    writeln(x);
  }
}

var f : foo = foo();

f.x = 2;

f.print();

var f2 : foo = foo();

f2.x = 3.2;

f2.print();
