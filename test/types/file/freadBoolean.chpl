var thisIsTrue: bool = false;
var thisIsFalse: bool = true;

var f: file = file(filename = "freadBoolean.txt", mode = "r");
f.open;

f.read(thisIsTrue, thisIsFalse);
writeln(thisIsTrue);
writeln(thisIsFalse);
