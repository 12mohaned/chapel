var x = foo();
var y = -1;

function main(argc: int, argv: [1..argc] string) {
  var y = 42;

  writeln("x is: ", x);
  writeln("y is: ", y);  
}

function foo() {
  writeln("If we see this, I think it worked");
}
