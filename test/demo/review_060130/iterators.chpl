config var n : int = 4;

var d : domain(2) = [1..n, 1..n];

var a : [d] int;

forall i,j in d do
  a(i,j) = (i-1)*n+j-1;

writeln(a);

iterator rmo(d : domain(2)) : (int,int)
  forall i in d.range(1) do
    forall j in d.range(2) do
      yield (i,j);

iterator cmo(d : domain(2)) : (int,int)
  forall j in d.range(2) do
    forall i in d.range(1) do
      yield (i,j);

for i in rmo(d) do
  writeln(a(i));

for i in cmo(d) do
  writeln(a(i));
