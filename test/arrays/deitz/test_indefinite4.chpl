var d : domain(integer);
var a : [d] integer;

d.add(2);
d.add(5);

a(2) = 7;
a(5) = 4;

writeln(d);
writeln(a);

for i in d do
  writeln(i, " -> ", a(i));
