var isBushAnIdiot: bool;
var isBushALiar:   bool;

writeln("Is Bush an idiot? (true or false)");
read(isBushAnIdiot);
writeln(isBushAnIdiot);
while (not isBushAnIdiot) {
  writeln("That's the wrong answer, try again.");
  read(isBushAnIdiot);
}
writeln(isBushAnIdiot);
writeln("Is Bush a liar? (true or false)");
read(isBushALiar);
