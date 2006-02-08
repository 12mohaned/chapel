config var n : integer = 4;

var D : domain(2) = [1..n, 1..n];
var A : [D] integer;
var B : [D] integer;
var C : [D] integer;

[i,j in D] A(i,j) = (i - 1) * n + j;
[i,j in D] B(i,j) = (i - 1) * n + j;

writeln(A);
writeln(B);

C = A + B;

A(1,1) = 500;
B(1,1) = 500;

writeln(C);
