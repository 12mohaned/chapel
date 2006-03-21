class Cons {
  var car;
  var cdr; 
};

fun print(c : Cons) {
  if (c != nil) {
     writeln(c.car);
     print(c.cdr);
  }
}

var a = Cons(1, Cons(2, nil));
var b = Cons(1.0, Cons(2.0, nil));

print(a);
print(b);
