class mysumreduce {
  type intype;
  type statetype;
  type outtype;
  
  def ident(): statetype {
    return 0;
  }

  def combine(x: statetype, y: intype): statetype {
    return x + y;
  }

  def result(x: statetype): outtype {
    return x;
  }

  constructor mysumreduce(type in_t) {
    intype = in_t;
    statetype = in_t;
    outtype = in_t;
  }
}

config var n: int = 10;

var D: domain(1) = [1..n];

var A: [D] int;

forall i in D {
  A(i) = i;
}

var myreduce = mysumreduce(in_t = int);
var state: myreduce.statetype = myreduce.ident();
for i in D {
  state = myreduce.combine(state, A(i));
}
var result = state;

writeln("result is: ", result);
