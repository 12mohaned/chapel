-- NOTE:  This test prints a.x = 1 when it should print a.x = 9.
-- shannon, finish futurizing this.  commit.

class myClass {
  var x: int;
  var y: real;
}

var a: myClass = myClass(x = 1, y = 2.3);
var myInt: int = 9;
var myFile: file = file(filename = "_test_freadToClassDataMember.txt", mode = "w");

myFile.open;
fwriteln(myFile, myInt);
myFile.close;

myFile.mode = "r";
myFile.open;
fread(myFile, a.x);
myFile.close;

writeln("a.x should be equal to 9");
writeln("a.x = ", a.x);

