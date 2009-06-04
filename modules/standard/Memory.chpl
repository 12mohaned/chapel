enum MemUnits {Bytes, KB, MB, GB};

def locale.physicalMemory(unit: MemUnits=MemUnits.Bytes, type retType=int(64)) {
  var bytesInLocale: uint(64);

  on this do bytesInLocale = __primitive("chpl_bytesPerLocale");

  var retVal: retType;
  select (unit) {
    when MemUnits.Bytes do retVal = bytesInLocale:retType;
    when MemUnits.KB do retVal = (bytesInLocale:retType / 1024):retType;
    when MemUnits.MB do retVal = (bytesInLocale:retType / (1024**2)):retType;
    when MemUnits.GB do retVal = (bytesInLocale:retType / (1024**3)):retType;
  }

  return retVal;
}

def memoryUsed()
  return __primitive("chpl_memoryUsed");

def printMemTable(thresh=0) {
  __primitive("chpl_printMemTable", thresh);
}

def printMemStat() {
  __primitive("chpl_printMemStat");
}

def startVerboseMem() { __primitive("chpl_startVerboseMem"); }
def stopVerboseMem() { __primitive("chpl_stopVerboseMem"); }
def startVerboseMemHere() { __primitive("chpl_startVerboseMemHere"); }
def stopVerboseMemHere() { __primitive("chpl_stopVerboseMemHere"); }
