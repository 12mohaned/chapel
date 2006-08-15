// test #1
var s:int;

def function1( a:int, b:int, c:int) {
  var d:int;

  def nested_function( a:int, b:int, c:int) {
    writeln( "test1:", a, b, c, d, s);
  }

  d = 2*a;
  cobegin {
    nested_function( a, b, c);
    nested_function( b, c, a);
    nested_function( c, a, b);
  }
}

var x:int = 1;
var y:int = 2;
var z:int = 3;

s = 5;
function1( x, y, z);


// test #2
def function2( a:int) {
  var d:int;

  d = a;
  cobegin {
    d = 2*a;
    d = 4*a;
  }
  write( "test2: ");
  if (d==4 || d==8) {
    writeln( "good");
  } else {
    writeln( "bad");
  }
}

function2( 2);


// test #3
def function3() {
  class C {
    var a:int;
  }

  class D {
    var c:C;
  }

  var c:C = C();
  c.a = 1;

  var d:D = D();
  d.c = C();
  d.c.a = 2;

  cobegin {
    {
      d.c.a = c.a * 6;
      c.a = 5;
    }
  }

  writeln( "test3:", c, d);
}

function3();
