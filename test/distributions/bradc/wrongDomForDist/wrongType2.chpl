use DummyArithDist;

var Dist = new dist(new MyDist());
var DArith: domain(1) distributed(Dist);
var DAssoc: domain(opaque) distributed(Dist);
