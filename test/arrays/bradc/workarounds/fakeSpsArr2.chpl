record sps33 {
  var data:[-1..1, 0..1] real;

  def this(i, j) {
    if (i == j) {
      return 0.0;
    } else if (i==-1) {
      return data(i, j==1);
    } else {
      return data(i, j!=-1);
    }
  }

  def =this(i, j, v) {
    if (i == j) {
      halt("Assigning an IRV value");
    } else if (i==-1) {
      data(i, j==1) = v;
    } else {
      data(i, j!=-1) = v;
    }
  }

  def this(ij: 2*int) {
    return this(ij(1), ij(2));
  }

  def =this(ij: 2*int) {
    return this(ij(1), ij(2));
  }
}

iterator SpsStencDom() {
  for (i,j) in [-1..1, -1..1] do
    if (i != j) then
      yield (i,j);
}


var a: sps33;

for (i,j) in SpsStencDom() {
  a(i,j) = (i+1)*3 + j+2;
}

for i in -1..1 {
  for j in -1..1 {
    write(a(i,j), " ");
  }
  writeln();
}
