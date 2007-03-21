//
// assignment on primitive types
//
pragma "inline" def =(a: bool, b: bool) return b;
pragma "inline" def =(a: int(?w), b: int(w)) return b;
pragma "inline" def =(a: uint(?w), b: uint(w)) return b;
pragma "inline" def =(a: real(?w), b: real(w)) return b;
pragma "inline" def =(a: imag(?w), b: imag(w)) return b;
pragma "inline" def =(a: complex(?w), b: complex(w)) return b;
pragma "inline" def =(a: string, b: string) return __primitive("string_copy", b);
pragma "inline" def =(a, b) return b;

//
// equality comparison on primitive types
//
pragma "inline" def ==(a: bool, b: bool) return __primitive("==", a, b);
pragma "inline" def ==(a: int(32), b: int(32)) return __primitive("==", a, b);
pragma "inline" def ==(a: int(64), b: int(64)) return __primitive("==", a, b);
pragma "inline" def ==(a: uint(32), b: uint(32)) return __primitive("==", a, b);
pragma "inline" def ==(a: uint(64), b: uint(64)) return __primitive("==", a, b);
pragma "inline" def ==(a: real(?w), b: real(w)) return __primitive("==", a, b);
pragma "inline" def ==(a: string, b: string) return __primitive("string_equal", a, b);
pragma "inline" def ==(a: object, b: object) return __primitive("ptr_eq", a, b);

pragma "inline" def !=(a: bool, b: bool) return __primitive("!=", a, b);
pragma "inline" def !=(a: int(32), b: int(32)) return __primitive("!=", a, b);
pragma "inline" def !=(a: int(64), b: int(64)) return __primitive("!=", a, b);
pragma "inline" def !=(a: uint(32), b: uint(32)) return __primitive("!=", a, b);
pragma "inline" def !=(a: uint(64), b: uint(64)) return __primitive("!=", a, b);
pragma "inline" def !=(a: real(?w), b: real(w)) return __primitive("!=", a, b);
pragma "inline" def !=(a: string, b: string) return !(a == b);
pragma "inline" def !=(a: object, b: object) return __primitive("ptr_neq", a, b);

pragma "inline" def ==(param a: bool, param b: bool) param return __primitive("==", a, b);
pragma "inline" def ==(param a: int(32), param b: int(32)) param return __primitive("==", a, b);
pragma "inline" def ==(param a: int(64), param b: int(64)) param return __primitive("==", a, b);
pragma "inline" def ==(param a: uint(32), param b: uint(32)) param return __primitive("==", a, b);
pragma "inline" def ==(param a: uint(64), param b: uint(64)) param return __primitive("==", a, b);
pragma "inline" def ==(param a: enumerated, param b: enumerated) param return __primitive("==", a, b);

pragma "inline" def !=(param a: bool, param b: bool) param return __primitive("!=", a, b);
pragma "inline" def !=(param a: int(32), param b: int(32)) param return __primitive("!=", a, b);
pragma "inline" def !=(param a: int(64), param b: int(64)) param return __primitive("!=", a, b);
pragma "inline" def !=(param a: uint(32), param b: uint(32)) param return __primitive("!=", a, b);
pragma "inline" def !=(param a: uint(64), param b: uint(64)) param return __primitive("!=", a, b);
pragma "inline" def !=(param a: enumerated, param b: enumerated) param return __primitive("!=", a, b);
//
// ordered comparison on primitive types
//
pragma "inline" def <=(a: int(32), b: int(32)) return __primitive("<=", a, b);
pragma "inline" def <=(a: int(64), b: int(64)) return __primitive("<=", a, b);
pragma "inline" def <=(a: uint(32), b: uint(32)) return __primitive("<=", a, b);
pragma "inline" def <=(a: uint(64), b: uint(64)) return __primitive("<=", a, b);
pragma "inline" def <=(a: real(?w), b: real(w)) return __primitive("<=", a, b);

pragma "inline" def >=(a: int(32), b: int(32)) return __primitive(">=", a, b);
pragma "inline" def >=(a: int(64), b: int(64)) return __primitive(">=", a, b);
pragma "inline" def >=(a: uint(32), b: uint(32)) return __primitive(">=", a, b);
pragma "inline" def >=(a: uint(64), b: uint(64)) return __primitive(">=", a, b);
pragma "inline" def >=(a: real(?w), b: real(w)) return __primitive(">=", a, b);

pragma "inline" def <(a: int(32), b: int(32)) return __primitive("<", a, b);
pragma "inline" def <(a: int(64), b: int(64)) return __primitive("<", a, b);
pragma "inline" def <(a: uint(32), b: uint(32)) return __primitive("<", a, b);
pragma "inline" def <(a: uint(64), b: uint(64)) return __primitive("<", a, b);
pragma "inline" def <(a: real(?w), b: real(w)) return __primitive("<", a, b);

pragma "inline" def >(a: int(32), b: int(32)) return __primitive(">", a, b);
pragma "inline" def >(a: int(64), b: int(64)) return __primitive(">", a, b);
pragma "inline" def >(a: uint(32), b: uint(32)) return __primitive(">", a, b);
pragma "inline" def >(a: uint(64), b: uint(64)) return __primitive(">", a, b);
pragma "inline" def >(a: real(?w), b: real(w)) return __primitive(">", a, b);

pragma "inline" def <=(param a: int(32), param b: int(32)) param return __primitive("<=", a, b);
pragma "inline" def <=(param a: int(64), param b: int(64)) param return __primitive("<=", a, b);
pragma "inline" def <=(param a: uint(32), param b: uint(32)) param return __primitive("<=", a, b);
pragma "inline" def <=(param a: uint(64), param b: uint(64)) param return __primitive("<=", a, b);
pragma "inline" def <=(param a: enumerated, param b: enumerated) param return __primitive("<=", a, b);

pragma "inline" def >=(param a: int(32), param b: int(32)) param return __primitive(">=", a, b);
pragma "inline" def >=(param a: int(64), param b: int(64)) param return __primitive(">=", a, b);
pragma "inline" def >=(param a: uint(32), param b: uint(32)) param return __primitive(">=", a, b);
pragma "inline" def >=(param a: uint(64), param b: uint(64)) param return __primitive(">=", a, b);
pragma "inline" def >=(param a: enumerated, param b: enumerated) param return __primitive(">=", a, b);


pragma "inline" def <(param a: int(32), param b: int(32)) param return __primitive("<", a, b);
pragma "inline" def <(param a: int(64), param b: int(64)) param return __primitive("<", a, b);
pragma "inline" def <(param a: uint(32), param b: uint(32)) param return __primitive("<", a, b);
pragma "inline" def <(param a: uint(64), param b: uint(64)) param return __primitive("<", a, b);
pragma "inline" def <(param a: enumerated, param b: enumerated) param return __primitive("<", a, b);

pragma "inline" def >(param a: int(32), param b: int(32)) param return __primitive(">", a, b);
pragma "inline" def >(param a: int(64), param b: int(64)) param return __primitive(">", a, b);
pragma "inline" def >(param a: uint(32), param b: uint(32)) param return __primitive(">", a, b);
pragma "inline" def >(param a: uint(64), param b: uint(64)) param return __primitive(">", a, b);
pragma "inline" def >(param a: enumerated, param b: enumerated) param return __primitive(">", a, b);

//
// unary + and - on primitive types
//
pragma "inline" def +(a: int(32)) return a;
pragma "inline" def +(a: int(64)) return a;
pragma "inline" def +(a: uint(32)) return a;
pragma "inline" def +(a: uint(64)) return a;
pragma "inline" def +(a: real(?w)) return a;
pragma "inline" def +(a: imag(?w)) return a;
pragma "inline" def +(a: complex(?w)) return a;

pragma "inline" def -(a: int(32)) return __primitive("u-", a);
pragma "inline" def -(a: int(64)) return __primitive("u-", a);
pragma "inline" def -(a: real(?w)) return __primitive("u-", a);
pragma "inline" def -(a: imag(?w)) return __primitive("u-", a):imag(w);
pragma "inline" def -(a: complex(?w)) return (-a.re, -a.im):complex;

pragma "inline" def +(param a: int(32)) param return a;
pragma "inline" def +(param a: int(64)) param return a;
pragma "inline" def +(param a: uint(32)) param return a;
pragma "inline" def +(param a: uint(64)) param return a;

pragma "inline" def -(param a: int(32)) param return __primitive("u-", a);
pragma "inline" def -(param a: int(64)) param return __primitive("u-", a);

//
// binary + and - on primitive types
//
pragma "inline" def +(a: int(32), b: int(32)) return __primitive("+", a, b);
pragma "inline" def +(a: int(64), b: int(64)) return __primitive("+", a, b);
pragma "inline" def +(a: uint(32), b: uint(32)) return __primitive("+", a, b);
pragma "inline" def +(a: uint(64), b: uint(64)) return __primitive("+", a, b);
pragma "inline" def +(a: real(?w), b: real(w)) return __primitive("+", a, b);
pragma "inline" def +(a: imag(?w), b: imag(w)) return __primitive("+", a, b);
pragma "inline" def +(a: complex(?w), b: complex(w)) return (a.re+b.re, a.im+b.im):complex;
pragma "inline" def +(a: string, b: string) return __primitive("string_concat", a, b);

pragma "inline" def +(a: real(?w), b: imag(w)) return (a, _i2r(b)):complex;
pragma "inline" def +(a: imag(?w), b: real(w)) return (b, _i2r(a)):complex;
pragma "inline" def +(a: real(?w), b: complex(w*2)) return (a+b.re, b.im):complex;
pragma "inline" def +(a: complex(?w), b: real(w/2)) return (a.re+b, a.im):complex;
pragma "inline" def +(a: imag(?w), b: complex(w*2)) return (b.re, _i2r(a)+b.im):complex;
pragma "inline" def +(a: complex(?w), b: imag(w/2)) return (a.re, a.im+_i2r(b)):complex;

pragma "inline" def -(a: int(32), b: int(32)) return __primitive("-", a, b);
pragma "inline" def -(a: int(64), b: int(64)) return __primitive("-", a, b);
pragma "inline" def -(a: uint(32), b: uint(32)) return __primitive("-", a, b);
pragma "inline" def -(a: uint(64), b: uint(64)) return __primitive("-", a, b);
pragma "inline" def -(a: real(?w), b: real(w)) return __primitive("-", a, b);
pragma "inline" def -(a: imag(?w), b: imag(w)) return __primitive("-", a, b);
pragma "inline" def -(a: complex(?w), b: complex(w)) return (a.re-b.re, a.im-b.im):complex;

pragma "inline" def -(a: real(?w), b: imag(w)) return (a, -_i2r(b)):complex;
pragma "inline" def -(a: imag(?w), b: real(w)) return (-b, _i2r(a)):complex;
pragma "inline" def -(a: real(?w), b: complex(w*2)) return (a-b.re, -b.im):complex;
pragma "inline" def -(a: complex(?w), b: real(w/2)) return (a.re-b, a.im):complex;
pragma "inline" def -(a: imag(?w), b: complex(w*2)) return (-b.re, _i2r(a)-b.im):complex;
pragma "inline" def -(a: complex(?w), b: imag(w/2)) return (a.re, a.im-_i2r(b)):complex;

pragma "inline" def +(param a: int(32), param b: int(32)) param return __primitive("+", a, b);
pragma "inline" def +(param a: int(64), param b: int(64)) param return __primitive("+", a, b);
pragma "inline" def +(param a: uint(32), param b: uint(32)) param return __primitive("+", a, b);
pragma "inline" def +(param a: uint(64), param b: uint(64)) param return __primitive("+", a, b);

pragma "inline" def -(param a: int(32), param b: int(32)) param return __primitive("-", a, b);
pragma "inline" def -(param a: int(64), param b: int(64)) param return __primitive("-", a, b);
pragma "inline" def -(param a: uint(32), param b: uint(32)) param return __primitive("-", a, b);
pragma "inline" def -(param a: uint(64), param b: uint(64)) param return __primitive("-", a, b);

//
// * and / on primitive types
//
pragma "inline" def *(a: int(32), b: int(32)) return __primitive("*", a, b);
pragma "inline" def *(a: int(64), b: int(64)) return __primitive("*", a, b);
pragma "inline" def *(a: uint(32), b: uint(32)) return __primitive("*", a, b);
pragma "inline" def *(a: uint(64), b: uint(64)) return __primitive("*", a, b);
pragma "inline" def *(a: real(?w), b: real(w)) return __primitive("*", a, b);
pragma "inline" def *(a: imag(?w), b: imag(w)) return _i2r(__primitive("*", a, b));
pragma "inline" def *(a: complex(?w), b: complex(w)) return (a.re*b.re-a.im*b.im, a.im*b.re+a.re*b.im):complex;

pragma "inline" def *(a: real(?w), b: imag(w)) return (a*_i2r(b)):imag(w);
pragma "inline" def *(a: imag(?w), b: real(w)) return (_i2r(a)*b):imag(w);
pragma "inline" def *(a: real(?w), b: complex(w*2)) return (a*b.re, a*b.im):complex;
pragma "inline" def *(a: complex(?w), b: real(w/2)) return (a.re*b, a.im*b):complex;
pragma "inline" def *(a: imag(?w), b: complex(w*2)) return (-_i2r(a)*b.im, _i2r(a)*b.re):complex;
pragma "inline" def *(a: complex(?w), b: imag(w/2)) return (-a.im*_i2r(b), a.re*_i2r(b)):complex;

pragma "inline" def /(a: int(32), b: int(32)) return __primitive("/", a, b);
pragma "inline" def /(a: int(64), b: int(64)) return __primitive("/", a, b);
pragma "inline" def /(a: uint(32), b: uint(32)) return __primitive("/", a, b);
pragma "inline" def /(a: uint(64), b: uint(64)) return __primitive("/", a, b);
pragma "inline" def /(a: real(?w), b: real(w)) return __primitive("/", a, b);
pragma "inline" def /(a: imag(?w), b: imag(w)) return _i2r(__primitive("/", a, b));
pragma "inline" def /(a: complex(?w), b: complex(w))
  return let d = b.re*b.re+b.im*b.im in
    ((a.re*b.re+a.im*b.im)/d, (a.im*b.re-a.re*b.im)/d):complex;

pragma "inline" def /(a: real(?w), b: imag(w)) return (-a/_i2r(b)):imag(w);
pragma "inline" def /(a: imag(?w), b: real(w)) return (_i2r(a)/b):imag(w);
pragma "inline" def /(a: real(?w), b: complex(w*2))
  return let d = b.re*b.re+b.im*b.im in
    (a*b.re/d, -a*b.im/d):complex;
pragma "inline" def /(a: complex(?w), b: real(w/2))
  return (a.re/b, a.im/b):complex;
pragma "inline" def /(a: imag(?w), b: complex(w*2))
  return let d = b.re*b.re+b.im*b.im in
    (_i2r(a)*b.im/d, _i2r(a)*b.re/d):complex;
pragma "inline" def /(a: complex(?w), b: imag(w/2))
  return let d = _i2r(b)*_i2r(b) in
    (a.im/_i2r(b), -a.re/_i2r(b)):complex;

pragma "inline" def *(param a: int(32), param b: int(32)) param return __primitive("*", a, b);
pragma "inline" def *(param a: int(64), param b: int(64)) param return __primitive("*", a, b);
pragma "inline" def *(param a: uint(32), param b: uint(32)) param return __primitive("*", a, b);
pragma "inline" def *(param a: uint(64), param b: uint(64)) param return __primitive("*", a, b);

pragma "inline" def /(param a: int(32), param b: int(32)) param return __primitive("/", a, b);
pragma "inline" def /(param a: int(64), param b: int(64)) param return __primitive("/", a, b);
pragma "inline" def /(param a: uint(32), param b: uint(32)) param return __primitive("/", a, b);
pragma "inline" def /(param a: uint(64), param b: uint(64)) param return __primitive("/", a, b);

//
// % on primitive types
//
pragma "inline" def %(a: int(32), b: int(32)) return __primitive("%", a, b);
pragma "inline" def %(a: int(64), b: int(64)) return __primitive("%", a, b);
pragma "inline" def %(a: uint(32), b: uint(32)) return __primitive("%", a, b);
pragma "inline" def %(a: uint(64), b: uint(64)) return __primitive("%", a, b);

pragma "inline" def %(param a: int(32), param b: int(32)) param return __primitive("%", a, b);
pragma "inline" def %(param a: int(64), param b: int(64)) param return __primitive("%", a, b);
pragma "inline" def %(param a: uint(32), param b: uint(32)) param return __primitive("%", a, b);
pragma "inline" def %(param a: uint(64), param b: uint(64)) param return __primitive("%", a, b);

//
// ** on primitive types
//
pragma "inline" def **(a: int(32), b: int(32)) return __primitive("**", a, b);
pragma "inline" def **(a: int(64), b: int(64)) return __primitive("**", a, b);
pragma "inline" def **(a: uint(32), b: uint(32)) return __primitive("**", a, b);
pragma "inline" def **(a: uint(64), b: uint(64)) return __primitive("**", a, b);
pragma "inline" def **(a: real(?w), b: real(w)) return __primitive("**", a, b);

pragma "inline" def **(param a: int(32), param b: int(32)) param return __primitive("**", a, b);
pragma "inline" def **(param a: int(64), param b: int(64)) param return __primitive("**", a, b);
pragma "inline" def **(param a: uint(32), param b: uint(32)) param return __primitive("**", a, b);
pragma "inline" def **(param a: uint(64), param b: uint(64)) param return __primitive("**", a, b);

//
// logical operations on primitive types
//
pragma "inline" def !(a: bool) return __primitive("!", a);
pragma "inline" def bool.true? return this;

pragma "inline" def !(param a: bool) param return __primitive("!", a);

//
// bitwise operations on primitive types
//
pragma "inline" def ~(a: bool) return __primitive("u~", a);
pragma "inline" def ~(a: int(32)) return __primitive("u~", a);
pragma "inline" def ~(a: int(64)) return __primitive("u~", a);
pragma "inline" def ~(a: uint(32)) return __primitive("u~", a);
pragma "inline" def ~(a: uint(64)) return __primitive("u~", a);

pragma "inline" def &(a: bool, b: bool) return __primitive("&", a, b);
pragma "inline" def &(a: int(32), b: int(32)) return __primitive("&", a, b);
pragma "inline" def &(a: int(64), b: int(64)) return __primitive("&", a, b);
pragma "inline" def &(a: uint(32), b: uint(32)) return __primitive("&", a, b);
pragma "inline" def &(a: uint(64), b: uint(64)) return __primitive("&", a, b);

pragma "inline" def |(a: bool, b: bool) return __primitive("|", a, b);
pragma "inline" def |(a: int(32), b: int(32)) return __primitive("|", a, b);
pragma "inline" def |(a: int(64), b: int(64)) return __primitive("|", a, b);
pragma "inline" def |(a: uint(32), b: uint(32)) return __primitive("|", a, b);
pragma "inline" def |(a: uint(64), b: uint(64)) return __primitive("|", a, b);

pragma "inline" def ^(a: bool, b: bool) return __primitive("^", a, b);
pragma "inline" def ^(a: int(32), b: int(32)) return __primitive("^", a, b);
pragma "inline" def ^(a: int(64), b: int(64)) return __primitive("^", a, b);
pragma "inline" def ^(a: uint(32), b: uint(32)) return __primitive("^", a, b);
pragma "inline" def ^(a: uint(64), b: uint(64)) return __primitive("^", a, b);

pragma "inline" def ~(param a: bool) param return __primitive("u~", a);
pragma "inline" def ~(param a: int(32)) param return __primitive("u~", a);
pragma "inline" def ~(param a: int(64)) param return __primitive("u~", a);
pragma "inline" def ~(param a: uint(32)) param return __primitive("u~", a);
pragma "inline" def ~(param a: uint(64)) param return __primitive("u~", a);

pragma "inline" def &(param a: bool, param b: bool) param return __primitive("&", a, b);
pragma "inline" def &(param a: int(32), param b: int(32)) param return __primitive("&", a, b);
pragma "inline" def &(param a: int(64), param b: int(64)) param return __primitive("&", a, b);
pragma "inline" def &(param a: uint(32), param b: uint(32)) param return __primitive("&", a, b);
pragma "inline" def &(param a: uint(64), param b: uint(64)) param return __primitive("&", a, b);

pragma "inline" def |(param a: bool, param b: bool) param return __primitive("|", a, b);
pragma "inline" def |(param a: int(32), param b: int(32)) param return __primitive("|", a, b);
pragma "inline" def |(param a: int(64), param b: int(64)) param return __primitive("|", a, b);
pragma "inline" def |(param a: uint(32), param b: uint(32)) param return __primitive("|", a, b);
pragma "inline" def |(param a: uint(64), param b: uint(64)) param return __primitive("|", a, b);

pragma "inline" def ^(param a: bool, param b: bool) param return __primitive("^", a, b);
pragma "inline" def ^(param a: int(32), param b: int(32)) param return __primitive("^", a, b);
pragma "inline" def ^(param a: int(64), param b: int(64)) param return __primitive("^", a, b);
pragma "inline" def ^(param a: uint(32), param b: uint(32)) param return __primitive("^", a, b);
pragma "inline" def ^(param a: uint(64), param b: uint(64)) param return __primitive("^", a, b);

//
// left and right shift on primitive types
//
pragma "inline" def <<(a: int(32), b: integral) return __primitive("<<", a, b);
pragma "inline" def <<(a: int(64), b: integral) return __primitive("<<", a, b);
pragma "inline" def <<(a: uint(32), b: integral) return __primitive("<<", a, b);
pragma "inline" def <<(a: uint(64), b: integral) return __primitive("<<", a, b);

pragma "inline" def >>(a: int(32), b: integral) return __primitive(">>", a, b);
pragma "inline" def >>(a: int(64), b: integral) return __primitive(">>", a, b);
pragma "inline" def >>(a: uint(32), b: integral) return __primitive(">>", a, b);
pragma "inline" def >>(a: uint(64), b: integral) return __primitive(">>", a, b);

pragma "inline" def <<(param a: int(32), param b: integral) param return __primitive("<<", a, b);
pragma "inline" def <<(param a: int(64), param b: integral) param return __primitive("<<", a, b);
pragma "inline" def <<(param a: uint(32), param b: integral) param return __primitive("<<", a, b);
pragma "inline" def <<(param a: uint(64), param b: integral) param return __primitive("<<", a, b);

pragma "inline" def >>(param a: int(32), param b: integral) param return __primitive(">>", a, b);
pragma "inline" def >>(param a: int(64), param b: integral) param return __primitive(">>", a, b);
pragma "inline" def >>(param a: uint(32), param b: integral) param return __primitive(">>", a, b);
pragma "inline" def >>(param a: uint(64), param b: integral) param return __primitive(">>", a, b);

//
// _init on primitive types and classes
//
pragma "inline" def _init(x: bool) return false;
pragma "inline" def _init(x: int(?w)) return 0:int(w);
pragma "inline" def _init(x: uint(?w)) return 0:uint(w);
pragma "inline" def _init(x: real(?w)) return 0.0:real(w);
pragma "inline" def _init(x: imag(?w)) return 0.0:imag(w);
pragma "inline" def _init(x: complex(?w)) return (0.0:real(w/2), 0.0:real(w/2)):complex;
pragma "inline" def _init(x: string) return "";
pragma "inline" def _init(x) return nil:x;

//
// _copy on primitive types and classes
//
pragma "inline" def _copy(a) return a;

pragma "inline" def _pass(a) return _copy(a);

//
// _cond_test function supports statement bool conversions and sync
//   variables in conditional statements
//
pragma "inline" def _cond_test(x: object) return x != nil;
pragma "inline" def _cond_test(x: bool) return x;
pragma "inline" def _cond_test(x: integral) return x != 0;

pragma "inline" def _cond_test(param x: bool) param return x;
pragma "inline" def _cond_test(param x: integral) param return x != 0;

pragma "inline" def _cond_test(x) {
  compilerError("illegal type '", x.type, "' used in if or while condition");
}

//
// complex component methods re and im
//
//  bug?  in setters, parameterize real argument over complex bit width
//
def complex.re return __primitive( "complex_get_real", this);
def complex.im return __primitive( "complex_get_imag", this);
def complex.=re(f:real) { __primitive( "complex_set_real", this, f); }
def complex.=im(f:real) { __primitive( "complex_set_imag", this, f); }

//
// helper functions
//
pragma "inline" def _i2r(a: imag(?w)) return a:real(w);

//
// primitive string functions and methods
//
pragma "inline" def ascii(a: string) return __primitive("ascii", a);
pragma "inline" def length(a: string) return __primitive("string_length", a);
pragma "inline" def string.substring(i: int) return __primitive("string_index", this, i);
pragma "inline" def _string_contains(a: string, b: string) return __primitive("string_contains", a, b);

//
// identity functions (for reductions)
//
pragma "inline" def _prod_id( type t) return __primitive( "_prod_id", t);
pragma "inline" def _land_id( type t) return __primitive( "_land_id", t);
pragma "inline" def _lor_id( type t) return __primitive( "_lor_id", t);
pragma "inline" def _lxor_id( type t) return __primitive( "_lxor_id", t);
pragma "inline" def _band_id( type t) return __primitive( "_band_id", t);
pragma "inline" def _bor_id( type t) return __primitive( "_bor_id", t);
pragma "inline" def _bxor_id( type t) return __primitive( "_bxor_id", t);

//
// min and max
//
pragma "inline" def min(x, y) return if x < y then x else y;
pragma "inline" def max(x, y) return if x > y then x else y;
pragma "inline" def min(x, y, z...?k) return min(min(x, y), (...z));
pragma "inline" def max(x, y, z...?k) return max(max(x, y), (...z));

//
// More primitive funs
//
pragma "inline" def exit(status: int) {
  __primitive("exit", status);
}

pragma "inline" def _fflush(fp: _file) return __primitive("fflush", fp);

def init_elts(x, s, type t) {
  for i in 1..s {
    var y: t;
    __primitive("array_set_first", x, i-1, y);
  }
}

// dynamic data block class
pragma "data class"
class _ddata {
  type eltType;
  var size: int;
  pragma "inline" def init() {
    __primitive("array_init", this, eltType, size);
    init_elts(this, size, eltType);
  }
  pragma "inline" def init( d: eltType) {
    __primitive("array_init", this, eltType, size);
    [i in 0..size-1] __primitive("array_set_first", this, i, d);
  }
  pragma "inline" def this(i: int) {
    return __primitive("array_get", this, i);
  }
  pragma "inline" def =this(i: int, val: eltType) {
    __primitive("array_set", this, i, val);
  }
  pragma "data set error" pragma "inline" def =this(i: int, val) {
    compilerError("type mismatch");
  }
}


// synchronization primitives support
pragma "inline" def _init( cv: _mutex_p) return __primitive( "mutex_new");
pragma "inline" def =( a: _mutex_p, b: _mutex_p) return b;
pragma "inline" def _init( cv: _condvar_p) return __primitive( "condvar_new");
pragma "inline" def =( a: _condvar_p, b: _condvar_p) return b;


pragma "synchronization primitive" 
pragma "sync var"
pragma "no default functions"
pragma "no object"
class _syncvar {
  type base_type;
  var  value: base_type;     // actual data
  var  is_full: bool;
  var  lock: _mutex_p;       // need to acquire before accessing this record
  var  cv_empty: _condvar_p; // wait for empty; signal this when empty
  var  cv_full: _condvar_p;  // wait for full; signal this when full

  def initialize() {
    is_full = false; 
    lock = __primitive( "mutex_new");
    cv_empty = __primitive( "condvar_new");
    cv_full = __primitive( "condvar_new");
  }
}

pragma "synchronization primitive" 
def _copy( sv:_syncvar) {
  return readFE(sv);
}

pragma "synchronization primitive" 
def _pass( sv:_syncvar) {
  return sv;
}

pragma "synchronization primitive" 
def _init( sv:_syncvar) {
  return _syncvar( sv.value.type); 
}

// The operations are:
//  = (i.e., write_wait_empty_leave_full)
//  writeFE - wait for full, leave empty
//  writeXF - ignore F/E, leave full
//  writeXE - ignore F/E, leave empty
//  readFE - wait for full, leave empty
//  readFF - wait for full, leave full
//  readXF - ignore F/E, leave full
//  readXX - ignore F/E, leave F/E unchanged

// This is the default write on sync vars. Wait for empty, set and signal full.
pragma "synchronization primitive" 
def =( sv:_syncvar, value:sv.base_type) {
  __primitive( "sync_lock", sv);
  while (sv.is_full) {
    __primitive( "sync_wait_empty", sv);
  }
  sv.value = value;
  sv.is_full = true;
  __primitive( "sync_signal_full", sv);
  __primitive( "sync_unlock", sv);
  return sv;
}

// Wait for full, set and signal empty.
pragma "synchronization primitive" 
def writeFE( sv:_syncvar, value:sv.base_type) {
  __primitive( "sync_lock", sv);
  while (!sv.is_full) {
    __primitive( "sync_wait_full", sv);
  }
  sv.value = value;
  sv.is_full = false;
  __primitive( "sync_signal_empty", sv);
  __primitive( "sync_unlock", sv);
  return sv;
}

// Ignore F/E, set and signal full.
pragma "synchronization primitive" 
def writeXF( sv:_syncvar, value:sv.base_type) {
  __primitive( "sync_lock", sv);
  sv.value = value;
  sv.is_full = true;
  __primitive( "sync_signal_full", sv);
  __primitive( "sync_unlock", sv);
}

// Ignore F/E, set and signal leave empty. One use of this is with using 
// sync var as a lock.
pragma "synchronization primitive" 
def writeXE( sv:_syncvar, value:sv.base_type) {
  __primitive( "sync_lock", sv);
  sv.value = value;
  sv.is_full = false;
  __primitive( "sync_signal_full", sv);
  __primitive( "sync_unlock", sv);
}

// This is the default read on sync vars. Wait for full, set and signal empty. 
pragma "synchronization primitive" 
def readFE( sv:_syncvar) {
  var ret: sv.base_type;
  __primitive( "sync_lock", sv);
  while (!sv.is_full) {
    __primitive( "sync_wait_full", sv);
  }
  ret = sv.value;
  sv.is_full = false;
  __primitive( "sync_signal_empty", sv);
  __primitive( "sync_unlock", sv);
  return ret;
}

// Wait for full, set and signal full.
pragma "synchronization primitive" 
def readFF( sv:_syncvar) {
  var ret: sv.base_type;
  __primitive( "sync_lock", sv);
  while (!sv.is_full) {
    __primitive( "sync_wait_full", sv);
  }
  ret = sv.value;
  __primitive( "sync_signal_full", sv);
  __primitive( "sync_unlock", sv);
  return ret;
}

// Ignore F/E, set and signal full.
pragma "synchronization primitive" 
def readXF( sv:_syncvar) {
  var ret: sv.base_type;
  __primitive( "sync_lock", sv);
  ret = sv.value;
  __primitive( "sync_signal_full", sv);
  __primitive( "sync_unlock", sv);
  return ret;
}

// Ignore F/E.  Read value.  No state change or signals.
pragma "synchronization primitive" 
def readXX( sv:_syncvar) {
  var ret: sv.base_type;
  __primitive( "sync_lock", sv);
  ret = sv.value;
  __primitive( "sync_unlock", sv);
  return ret;
}


pragma "synchronization primitive" 
def isFull( sv:_syncvar) {
  var isfull: bool;
  __primitive( "sync_lock", sv);
  isfull = sv.is_full;
  __primitive( "sync_unlock", sv);
  return isfull;
}


// single variable support
pragma "synchronization primitive" 
pragma "single var" 
pragma "no default functions"
pragma "no object"
class _singlevar {
  type base_type;
  var  value: base_type;     // actual data
  var  is_full: bool;
  var  lock: _mutex_p;       // need to acquire before accessing this record
  var  cv_full: _condvar_p;  // wait for full

  def initialize() {
    is_full = false; 
    lock = __primitive( "mutex_new");
    cv_full = __primitive( "condvar_new");
  }
}

pragma "synchronization primitive" 
def _copy( sv:_singlevar) {
  return readFF( sv);
}

pragma "synchronization primitive" 
def _pass( sv:_singlevar) {
  return sv;
}

pragma "synchronization primitive" 
def _init( sv:_singlevar) {
  return _singlevar( sv.value.type); 
}


// Can only write once.  Otherwise, it is an error.
pragma "synchronization primitive" 
def =( sv:_singlevar, value:sv.base_type) {
  __primitive( "sync_lock", sv);
  if (sv.is_full) {
    halt( "single var already defined");
  }
  sv.value = value;
  sv.is_full = true;
  __primitive( "sync_signal_full", sv);
  __primitive( "sync_unlock", sv);
  return sv;
}


// Wait for full. Set and signal full.
pragma "synchronization primitive" 
def readFF( sv:_singlevar) {
  var ret: sv.base_type;
  __primitive( "sync_lock", sv);
  while (!sv.is_full) {
    __primitive( "sync_wait_full", sv);
  }
  ret = sv.value;
  __primitive( "sync_signal_full", sv); // in case others are waiting
  __primitive( "sync_unlock", sv);
  return ret;
}


//
// Functions on _file primitive type, the C file pointer type
//
pragma "inline" def _handleRuntimeError(s: string) {
  __primitive("printError", s);
}

pragma "inline" def _get_errno() return __primitive("get_errno");
pragma "inline" def _get_eof() return __primitive("get_eof");
pragma "inline" def _get_stdin() return __primitive("get_stdin");
pragma "inline" def _get_stdout() return __primitive("get_stdout");
pragma "inline" def _get_stderr() return __primitive("get_stderr");
pragma "inline" def _get_nullfile() return __primitive("get_nullfile");

pragma "inline" def _copy(x: _file) return x;
pragma "inline" def =(a: _file, b: _file) return b;
pragma "inline" def ==(a: _file, b: _file) return __primitive("==", a, b);
pragma "inline" def !=(a: _file, b: _file) return __primitive("!=", a, b);

pragma "inline" def _fopen(filename: string, mode: string)
  return __primitive("fopen", filename, mode);

pragma "inline" def _fclose(fp: _file)
  return __primitive("fclose", fp);

pragma "inline" def fprintf(fp: _file, fmt: string, val)
  return __primitive("fprintf", fp, fmt, val);

pragma "inline" def fscanf(fp: _file, fmt: string, inout val)
  return __primitive("fscanf", fp, fmt, val);

pragma "inline" def _string_fscanf(fp: _file)
  return __primitive("string_fscanf", fp);

pragma "inline" def _readLitChar(fp: _file, val: string, ignoreWhiteSpace: bool)
  return __primitive("readLit", fp, val, ignoreWhiteSpace);


//
// casts
//
def _isPrimitiveType(type t) param return
  (t == bool) |
  (t == int(8)) | (t == int(16)) | (t == int(32)) | (t == int(64)) |
  (t == uint(8)) | (t == uint(16)) | (t == uint(32)) | (t == uint(64)) |
  (t == real(32)) | (t == real(64)) | (t == real(128)) |
  (t == imag(32)) | (t == imag(64)) | (t == imag(128)) |
  (t == complex(64)) | (t == complex(128)) | (t == complex(256)) |
  (t == string);

def _isIntegralType(type t) param return
  (t == int(8)) | (t == int(16)) | (t == int(32)) | (t == int(64)) |
  (t == uint(8)) | (t == uint(16)) | (t == uint(32)) | (t == uint(64));

pragma "inline" def _cast(type t, x: bool) where _isPrimitiveType(t)
  return __primitive("cast", t, x);

pragma "inline" def _cast(type t, x: int(?w)) where _isPrimitiveType(t)
  return __primitive("cast", t, x);

pragma "inline" def _cast(type t, x: uint(?w)) where _isPrimitiveType(t)
  return __primitive("cast", t, x);

pragma "inline" def _cast(type t, x: real(?w)) where _isPrimitiveType(t)
  return __primitive("cast", t, x);

pragma "inline" def _cast(type t, x: imag(?w)) where _isPrimitiveType(t)
  return __primitive("cast", t, x);

pragma "inline" def _cast(type t, x: complex(?w)) where _isPrimitiveType(t)
  return __primitive("cast", t, x);

pragma "inline" def _cast(type t, x: string) where _isPrimitiveType(t)
  return __primitive("cast", t, x);

pragma "inline" def _cast(type t, x: t) where !_isPrimitiveType(t)
  return x;

pragma "inline" def _cast(type t, x) where x:object & t:x
  return __primitive("dynamic_cast", t, x);

// inserts cast in compound assignments where necessary
def _compound_cast(type lt, type rt, x) where _isPrimitiveType(lt)
  return _cast(lt, x);

def _compound_cast(type lt, type rt, x) where !_isPrimitiveType(lt)
  return x;

  // note: need to verify that rhs can be assigned to lhs; checking
  // that rt:lt is not sufficient to handle parameter coercions

pragma "inline" def _chpl_memtest_printMemTable(threshold: int = 1000) { __primitive("_chpl_memtest_printMemTable", threshold); }
pragma "inline" def _chpl_memtest_printMemStat() { __primitive("_chpl_memtest_printMemStat"); }
pragma "inline" def _chpl_memtest_resetMemStat() { __primitive("_chpl_memtest_resetMemStat"); }
pragma "inline" def _chpl_memtest_allocAndFree() { __primitive("_chpl_memtest_allocAndFree"); }
pragma "inline" def _chpl_memtest_freedMalloc() { __primitive("_chpl_memtest_freedMalloc"); }
pragma "inline" def _chpl_memtest_freedWithoutMalloc() { __primitive("_chpl_memtest_freedWithoutMalloc"); }
pragma "inline" def _chpl_memtest_reallocWithoutMalloc() { __primitive("_chpl_memtest_reallocWithoutMalloc"); }
pragma "inline" def _chpl_memtest_reallocZeroSize() { __primitive("_chpl_memtest_reallocZeroSize"); }
pragma "inline" def _chpl_memtest_mallocOutOfMemory() { __primitive("_chpl_memtest_mallocOutOfMemory"); }
pragma "inline" def _chpl_memtest_reallocOutOfMemory() { __primitive("_chpl_memtest_reallocOutOfMemory"); }

pragma "inline" def _startTrackingMem() { __primitive("startTrackingMem"); }
