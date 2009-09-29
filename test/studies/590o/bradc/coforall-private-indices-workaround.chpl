config const sourceText = "<a><ii>end</ii><none /></a>";
const AllIndices: domain(1) = [1..length(sourceText)];
const AllPairs: domain(2) = [1..length(sourceText),
                             1..length(sourceText)];
var StartIndices: sparse subdomain(AllIndices);
var EndIndices: sparse subdomain(AllIndices);
var lock: sync int = 0;

def main {
  forall z in AllIndices do {
    if sourceText.substring[z] == '<' then {
      lock;
      StartIndices += z;
      if z > 1 && sourceText.substring[z-1] != ">" then
      EndIndices += z-1;
      lock = 0;
    }
    else if sourceText.substring[z] == '>' then {
      lock;
      EndIndices += z;
      if z < length(sourceText) &&
      sourceText.substring[z+1] != "<"  then StartIndices += z+1;
      lock = 0;
    }
  }

  var A: [AllPairs] real;
  coforall xy in [StartIndices, EndIndices] do {
    if (xy(1) <= xy(2)) {
      A[xy(1), xy(2)] = xy(1) + xy(2)/10.0;
    }
  }
  writeln("A is:\n", A);
  return;
}

