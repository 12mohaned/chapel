record point {
  var x : float;
  var y : float;
}

record cpoint {
  var color : int;
  with point;
}

var p : point = point();

p.x = 3.0;
p.y = 4.5;

writeln("p { ", p.x, ", ", p.y, " }");

var cp : cpoint = cpoint();

cp.color = 5;
cp.x = 2.2;
cp.y = 3.3;

writeln("cp { ", cp.x, ", ", cp.y, ", ", cp.color, " }");
