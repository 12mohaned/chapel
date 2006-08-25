record Matrix {
  type elt_type;
  var m, n: int;
  var D: domain(2) = [1..m, 1..n];
  var A: [D] elt_type;

  def this(i: int, j: int) var return A(i,j);
}

def fwrite(f: file, M: Matrix) {
  fwrite(f, M.A);
}

def Matrix.transpose() {
  var M: Matrix(elt_type, n, m);
  for i,j in D do
    M(j,i) = this(i,j);
  return M;
}

def +(M1: Matrix, M2: Matrix) {
  if M1.m != M2.m || M1.n != M2.n then
    halt("illegal matrix + operation");
  var M3: Matrix(M1(1,1)+M2(1,1), M1.m, M1.n);
  M3.A = M1.A + M2.A;
  return M3;
}

def *(M1: Matrix, M2: Matrix) {
  if M1.n != M2.m then
    halt("illegal matrix * operation");
  var M3: Matrix(M1(1,1)*M2(1,1), M1.m, M2.n);
  [i,j in M3.D] M3(i,j) = + reduce [k in M1.D(2)] (M1(i,k) + M2(k,j));
  return M3;
}

////////
// test code

var m = 8, n = 4;
var M: Matrix(float, m, n);
var N: Matrix(float, m, n);

for i,j in [1..m, 1..n] {
  M(i,j) = i-1 + (j-1)*m;
  N(i,j) = (i-1)*n + (j-1);
}

writeln(M);
writeln();
writeln(N);
writeln();
writeln(M+N);
writeln();
writeln(M.transpose());
writeln();
writeln(M*M.transpose());
