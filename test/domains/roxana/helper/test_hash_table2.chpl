class HashTable { 
  var size : int;
  var table: _ddata(int) = _ddata(int, 20); -- limitation
    -- you currently cannot use 'size' here
    -- eventually you may be able to especially if size is a param
  function initialize() {
    table.init();
  }
}  

var s1 : int = 10;

var ht : HashTable = HashTable(s1);

for i in 1..s1 do {
  ht.table(i-1) = 0;
}

-- writeln(ht); -- This needs to be changed, _data does not have a
                -- write function so you can't use the builtin write
                -- function for the class.

for i in 1..s1 do {
  writeln(ht.table(i-1));
}
