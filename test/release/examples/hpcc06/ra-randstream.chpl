module RARandomStream {
  param randWidth = 64;
  type randType = uint(randWidth);

  const bitDom = [0..randWidth),
        m2: [bitDom] randType = computeM2Vals(randWidth);


  iterator RAStream(block) {
    var val = getNthRandom(block.low);
    for i in block {
      getNextRandom(val);
      yield val;
    }
  }


  def getNthRandom(in n) {
    param period = 0x7fffffffffffffff/7;
    
    n %= period;
    if (n == 0) then return 0x1;

    var ran: randType = 0x2;
    for i in [0..log2(n)) by -1 {
      var val: randType = 0;
      for j in bitDom do
        if ((ran >> j) & 1) then val ^= m2(j);
      ran = val;
      if ((n >> i) & 1) then getNextRandom(ran);
    }
    return ran;
  }


  def getNextRandom(inout x) {
    param POLY:randType = 0x7;
    param hiRandBit = 0x1:randType << (randWidth-1);

    x = (x << 1) ^ (if (x & hiRandBit) then POLY else 0);
  }


  iterator computeM2Vals(numVals) {
    var nextVal = 0x1: randType;
    for i in 1..numVals {
      yield nextVal;
      getNextRandom(nextVal);
      getNextRandom(nextVal);
    }
  }
}

