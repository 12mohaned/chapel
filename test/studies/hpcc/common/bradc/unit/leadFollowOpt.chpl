use BlockDist;

var Dist = distributionValue(new Block(rank=1, int(32), bbox=[1..9]));

var D1 = Dist.newArithmeticDom(1, int(32), false);
var D2 = Dist.newArithmeticDom(1, int(32), false);
D1.setIndices([1..9]);
D2.setIndices([2..10]);

forall (i,j) in (D1, D2) do
  writeln("(i,j) is: ", (i,j));
