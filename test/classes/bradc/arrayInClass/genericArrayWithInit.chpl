class D {
  def this(x:int) {
    return 1..10;
  }
}

class C {
  param rank: int;
  type t;
  var d: D;

  var n = 10;
  var Dom = [d(1)];
  var A: [Dom] t = 0;
}


var c = C(2,int);
writeln("c.A is: ", c.A);
