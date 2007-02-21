var a: int = 1;                      // module scope
var n: int = 14;

def b(c: int): int {        // def param scope
  var d: int = 4;                    // def scope
  writeln("c is: ", c);
  writeln("d is: ", d);

  return 2;
}

class e {
  var f: int = 6;                    // class scope
  def g(h: int): int {
    var i: int = 9;
//    writeln("f is: ", f);
    writeln("h is: ", h);
    writeln("i is: ", i);
    return 7;
  }
}

def main() {
  writeln("a is: ", a);
  var myB: int;
  myB = b(3);
  writeln("b(3) is: ", myB);
  var myE: e = e();
  var myG: int;
  myG = myE.g(8);
  writeln("e.g(8) is: ", myG);

/*  
  for j in 1..n {                        // for loop scope
    writeln("j is: ", j);
    var k: int = 11;
    writeln("k is: ", k);
  }
*/
// [l in 1..n] writeln("l is: ", l);     // forall expr scope
  {                                      // block stmt scope
    var m: int = 13;
    {
      var o: int = 15;

      writeln("m is: ", m);
      writeln("o is: ", o);
    }
  }

//  var q: int = let p = a+n in p*p;   // let expr scope
}
