class dog {
  var weight : real;
  var paws : int;
  def grow() {
    writeln("woof");
  }
}

var d : dog = dog();

d.weight = 38.5;
d.paws = 4;
writeln("d: (weight = ", d.weight, ", paws = ", d.paws, ")");
writeln("Grow");
d.grow();
writeln("d: (weight = ", d.weight, ", paws = ", d.paws, ")");
