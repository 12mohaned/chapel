class trio {
  type elt_type;

  var x1 : elt_type;
  var x2 : elt_type;
  var x3 : elt_type;

  def sum()
    return x1 + x2 + x3;
}

def trio.writeThis(f : Writer) {
  f.write("(", x1, " & ", x2, " & ", x3, ")");
}

var t1 : trio(int) = trio(int, 1, 2, 3);
var t2 : trio(real) = trio(real, x1=1.0);

writeln(t1);
writeln(t1.sum());
writeln(t2);
writeln(t2.sum());
