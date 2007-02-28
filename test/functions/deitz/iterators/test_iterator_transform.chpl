//
// This test illustrates the iterator transformation; the iterator foo
// is transformed into function goo and class bar.
//

//
// BEFORE
//
iterator foo(n: int) {
  var i = 0;
  while i < n {
    yield i;
    i = i + 1;
  }
}

for i in foo(10) do
  writeln(i);

//
// AFTER
//
def goo(n: int)
  return bar(n);

class bar {
  var n, i, result: int;
  def getNextCursor(c: int) {
    var n = this.n;
    var i = this.i;
    if c == 2 then
      goto L2;
    i = 0;
    this.i = 0;
    while i < n {
      this.result = i;
      return 2;
label L2
      i = i + 1;
      this.i = i;
    }
    return 0;
  }
  def getHeadCursor() return getNextCursor(1);
  def isValidCursor?(c: int) return c != 0;
  def getValue(c: int) return result;
}

for i in goo(10) do
  writeln(i);
