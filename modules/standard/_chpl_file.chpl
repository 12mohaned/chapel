// class file
//
//  chapel-level implementations of read, write, writeln
//  chapel-level implementations of assert, halt

const stdin  = file("stdin", "r", "/dev", _get_stdin());
const stdout = file("stdout", "w", "/dev", _get_stdout());
const stderr = file("stderr", "w", "/dev", _get_stderr());

class file: Writer {
  var filename : string = "";
  var mode : string = "r";
  var path : string = ".";
  var _fp : _file;
  var _lock : sync uint(64);    // for serializing output

  def open {
    if this == stdin || this == stdout || this == stderr then
      halt("***Error: It is not necessary to open \"", filename, "\"***");

    var fullFilename = path + "/" + filename;
    _fp = _fopen(fullFilename, mode);            

    if _fp == _get_nullfile() then
      halt("***Error: Unable to open \"", fullFilename, "\": ", _get_errno(), "***");
  }

  def _checkFileStateChangeLegality(state) {
    if (isOpen) {
      halt("Cannot change ", state, " of file ", path, "/", filename, 
           " while it is open");
    }
  }

  def filename {
    return filename;
  }

  def =filename(newfilename) {
    _checkFileStateChangeLegality("filename");
    filename = newfilename;
  }

  def path {
    return path;
  }

  def =path(newpath) {
    _checkFileStateChangeLegality("path");
    path = newpath;
  }

  def mode {
    return mode;
  }

  def =mode(newmode: string) {
    _checkFileStateChangeLegality("mode");
    mode = newmode;
  }

  def isOpen: bool {
    var openStatus: bool = false;
    if (_fp != _get_nullfile()) {
      openStatus = true;
    }
    return openStatus;
  }

  def close {
    if (this == stdin || this == stdout || this == stderr) {
      halt("***Error: You may not close \"", filename, "\"***");
    }
    if (_fp == _get_nullfile()) {
      var fullFilename = path + "/" + filename;
      halt("***Error: Trying to close \"", fullFilename, 
           "\" which isn't open***");
    }
    var returnVal: int = _fclose(_fp);
    if (returnVal < 0) {
      var fullFilename = path + "/" + filename;
      halt("***Error: The close of \"", fullFilename, "\" failed: ", 
           _get_errno(), "***");
    }
    _fp = _get_nullfile();
  }
}

def file.flush() {
  _fflush(_fp);
}

def _fopenError(f: file, isRead: bool) {
  var fullFilename:string = f.path + "/" + f.filename;
  if (isRead) {
    halt("***Error: You must open \"", fullFilename, 
         "\" before trying to read from it***");
  } else {
    halt("***Error: You must open \"", fullFilename, 
         "\" before trying to write to it***");
  }
}


def _fmodeError(f: file, isRead: bool) {
  var fullFilename: string = f.path + "/" + f.filename;
  var modestring = if (isRead) then "reading" else "writing";
  halt("***Error: ", fullFilename, " not open for ", modestring, "***");
}


def _fprintfError() {
  halt("***Error: Write failed: ", _get_errno(), "***");
}


def _fscanfError() {
  halt("***Error: Read failed: ", _get_errno(), "***");
}

def file.read(inout val: int) {
  if (isOpen) {
    var returnVal: int = fscanf(_fp, "%d", val);
    if (returnVal == _get_eof()) {
      halt("***Error: Read failed: EOF***");
    }
    if (returnVal < 0) {
      _fscanfError();
    } else if (returnVal == 0) {
      halt("***Error: No int was read***");
    }
  } else {
    _fopenError(this, isRead = true);
  }
}

def file.read(inout val: uint) {
  if (isOpen) {
    var returnVal: int = fscanf(_fp, "%llu", val);
    if (returnVal == _get_eof()) {
      halt("***Error: Read failed: EOF***");
    }
    if (returnVal < 0) {
      _fscanfError();
    } else if (returnVal == 0) {
      halt("***Error: No int was read***");
    }
  } else {
    _fopenError(this, isRead = true);
  }
}

def file.read(inout val: real) {
  if (isOpen) {
    var returnVal: int = fscanf(_fp, "%lg", val);
    if (returnVal == _get_eof()) {
      halt("***Error: Read failed: EOF***");
    }
    if (returnVal < 0) {
      _fscanfError();
    } else if (returnVal == 0) {
      halt("***Error: No real was read***");
    }
  } else {
    _fopenError(this, isRead = true);
  }
}

def file.read(inout val: complex) {
  var realPart: real;
  var imagPart: real;
  var imagI: string;
  var matchingCharWasRead: int;
  var isNeg: bool;

  read(realPart);
  matchingCharWasRead = _readLitChar(_fp, "+", true);
  if (matchingCharWasRead != 1) {
    matchingCharWasRead = _readLitChar(_fp, "-", true);
    if (matchingCharWasRead != 1) {
      halt("***Error: Incorrect format for complex numbers***");
    }
    isNeg = true;
  }

  read(imagPart);
  matchingCharWasRead = _readLitChar(_fp, "i", false);
  if (matchingCharWasRead != 1) {
    halt("***Error: Incorrect format for complex numbers***");
  }

  val.re = realPart;
  if (isNeg) {
    val.im = -imagPart;
  } else {
    val.im = imagPart;
  }
}

def file.read(inout val: string) {
  if (isOpen) {
    val = _string_fscanf(_fp);
  } else {
    _fopenError(this, isRead = true);
  }
}

def file.read(inout val: bool) {
  if (isOpen) {
    var valString : string = _string_fscanf(_fp);
    if (valString == "true") {
      val = true;
    } else if (valString == "false") {
      val = false;
    } else {
      halt("***Error: Not of bool type***");
    }
  } else {
    _fopenError(this, isRead = true);
  }
}

def string.writeThis(f: Writer) {
  f.writeIt(this);
}

def file.writeIt(s: string) {
  if !isOpen then
    _fopenError(this, isRead = false);
  if mode != "w" then
    _fmodeError(this, isRead = false);
  var returnVal = fprintf(_fp, "%s", s);
  if returnVal < 0 then
    _fprintfError();
}

class StringClass: Writer {
  var s: string;
  def writeIt(s: string) { this.s += s; }
}

pragma "ref this"
def string.write(args ...?n) {
  var sc = StringClass(this);
  sc.write((...args));
  this = sc.s;
}

def file.lockWrite() {
  var me: uint(64) = __primitive("thread_id");
  if isFull(_lock) then
    if readXX(_lock) == me then
      return false;
  _lock = me;
  return true;
}

def file.unlockWrite() {
  writeFE(_lock, 0); // this can also be 'writeXE' since _lock is full
}

class Writer {
  def writeIt(s: string);
  def lockWrite() return false;
  def unlockWrite();
  def write(args ...?n) {
    var need_release: bool = lockWrite();
    for param i in 1..n do
      args(i).writeThis(this);
    if need_release then unlockWrite();
  }
  def writeln(args ...?n) {
    write((...args), "\n");
  }
  def writeln() {
    write("\n");
  }
}

def write(args ...?n) {
  stdout.write((...args));
}

def writeln(args ...?n) {
  stdout.writeln((...args));
}

def writeln() {
  stdout.writeln();
}
