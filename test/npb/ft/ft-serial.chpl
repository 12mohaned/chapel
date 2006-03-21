// NAS FT - Ported to Chapel from ZPL

config const
  verbose         = false,
  timers_enabled  = false,
  timers_disabled = false;

const
  arand = 1220703125.0,
  seed  = 314159265.0,
  r23   = 0.5**23,
  t23   = 2.0**23,
  r46   = 0.5**46,
  t46   = 2.0**46;

function nextrandlc(inout x : float, a : float) : float {
  // SJD: I want to say these are all floats
  var t1, t2, t3, t4, a1, a2, x1, x2, z : float;
  t1 = r23 * a;
  a1 = floor(t1);
  a2 = a - t23 * a1;
  t1 = r23 * x;
  x1 = floor(t1);
  x2 = x - t23 * x1;
  t1 = a1 * x2 + a2 * x1;
  t2 = floor(r23 * t1);
  z  = t1 - t23 * t2;
  t3 = t23 * z + a2 * x2;
  t4 = floor(r46 * t3);
  x  = t3 - t46 * t4;
  return r46 * x;
}

function initrandlc(seed, a : float, in n : int) : float {
  var i : int, t, g : float;
  var x : float = seed;
  t = a;
  while n != 0 do {
    i = n / 2;
    if 2 * i != n then
      g = nextrandlc(x, t);
    g = nextrandlc(t, t);
    n = i;
  }
  return x;
}

var randlc_last_n : int = -2,
    randlc_last_x : double;
function randlc(n : int) : float {
  if n != randlc_last_n + 1 then
    randlc_last_x = initrandlc(seed, arand, n);
  randlc_last_n = n;
  return nextrandlc(randlc_last_x, arand);
}

enum classes {S = 1, W, A, B, C, D};

const class_defaults =
  (('T',    4,    4,    4,  3),
   ('S',   64,   64,   64,  6),
   ('W',   32,  128,  128,  6),
   ('A',  128,  256,  512,  6),
   ('B',  256,  256,  512, 20),
   ('C',  512,  512,  512, 20),
   ('D', 1024, 1024, 2048, 25));

config const problem_class = S;

--const (char, nx, ny, nz, niter) = class_defaults(problem_class);

--const
--  alpha = 1.0e-6,
--  pi = 3.141592653589793238,
--  epsilon = 1.0e-12;

function compute_initial_conditions(X1 : [?D] complex) {
  forall i,j,k in D {
    X1(i,j,k).real = randlc(((i*ny+j)*nz+k)*2);
    X1(i,j,k).imag = randlc(((i*ny+j)*nz+k)*2+1);
  }
}

function compute_index_map(Twiddle : [?D] float) {
  const ap = -4.0 * alpha * pi * pi;
  forall i,j,k in D do
    Twiddle = exp((ap*(i+nx/2) mod nx - nx/2)**2 +
                  (ap*(i+nx/2) mod nx - nx/2)**2 +
                  (ap*(i+nx/2) mod nx - nx/2)**2);
}

var fftblock : int = 16, fftblockpad : int = 18;

var u : [0..nz-1] dcomplex;

function fft_init() {
  var t, ti : double;
  var m = bpop(nz-1);
  var ku = 2;
  var ln = 1;
  u[0] = m;
  for j in 1..m {
    for i in 0..ln-1 {
      u[i+ku-1].real = cos(i*pi/ln);
      u[i+ku-1].imag = sin(i*pi/ln);
    }
    ku += ln;
    ln *= 2;
  }
}

function fftz2(dir, l, m, n, ny, ny1 : int,
               u : [0..nz-1] complex,
               x, y : [0..n-1, 0..ny1-1] complex) {
  var lk = 2**(l-1), li = 2**(m-1), lj = 2*lk;
  for i in 0..li-1 {
    var i11 = i * lk, i12 = i11 + n/2, i21 = i * lj, i22 = i21 + lk;
    var u1 = if dir == 1 then u(li+i) else conjg(u(li+i));
    for k in 0..lk-1 {
      for j in 0..ny-1 {
        var x11 = x[i11+k, j];
        var x21 = x[i12+k, j];
        y[i21+k, j] = x11+x21;
        y[i22+k, j] = u1*(x11-x21);
      }
    }
  }
}

function cfftz(dir, m, n, ny, ny1 : int,
               x, y : [0..n-1, 0..ny1-1] complex) {
  for l in 1..m by 2 {
    fftz2(dir, l, m, n, ny, ny1, u, x, y);
    if l != m then
      fftz2(dir, l+1, m, n, ny, ny1, u, y, x);
    else
      x = y;
  }
}

function cffts1(dir, n, X1, X2, ny, ny1, x, y) {
  for j in X1.domain(2) {
    for kk in X1.domain(3) by ny {
      [i1,_,i3 in 0..n-1,j..j,kk..kk+ny-1] x(i1,i3-kk) = X1(i1,j,i3);
      cfftz(dir, bpop(n-1), n, ny, ny1, x, y);
      [i1,_,i3 in 0..n-1,j..j,kk..kk+ny-1] X2(i1,j,i3) = x(i1,i3-kk);
    }
  }
}

function cffts2(dir, n, X1, X2, ny, ny1, x, y) {
  for i in X1.domain(1) {
    for kk in X1.domain(3) by ny {
      [_,i2,i3 in i..i,0..n-1,kk..kk+ny-1] x(i2,i3-kk) = X1(i,i2,i3);
      cfftz(dir, bpop(n-1), n, ny, ny1, x, y);
      [_,i2,i3 in i..i,0..n-1,kk..kk+ny-1] X2(i,i2,i3) = x(i2,i3-kk);
    }
  }
}

function cffts3(dir, n, X1, X2, ny, ny1, x, y) {
  for i in X1.domain(1) {
    for jj in X1.domain(2) by ny {
      [_,i2,i3 in i..i,jj..jj+ny-1,0..n-1] x[i3,i2-jj) = X1(i,i2,i3);
      cfftz(dir, bpop(n-1), n, ny, ny1, x, y);
      [_,i2,i3 in i..i,jj..jj+ny-1,0..n-1] X2(i,i2,i3) = x[i3,i2-jj);
    }
  }
}

function fft(dir : int, X1, X2) {
  var x, y : [0..nz-1, 0..fftblockpad-1] complex;
  if dir == 1 {
    cffts3(dir, nz, X1, X1, fftblock, fftblockpad, x, y);
    cffts2(dir, ny, X1, X1, fftblock, fftblockpad, x, y);
    cffts1(dir, nx, X1, X2, fftblock, fftblockpad, x, y);
  } else {
    cffts1(dir, nx, X1, X1, fftblock, fftblockpad, x, y);
    cffts2(dir, ny, X1, X1, fftblock, fftblockpad, x, y);
    cffts3(dir, nz, X1, X2, fftblock, fftblockpad, x, y);
  }
}

function evolve(X1, X2, Twiddle) {
  X1 *= Twiddle;
  X2 = X1;
}

var sums : [1..niter] complex; -- verification checksums

const
  vdata_s : [1..6] complex =
    (/ 5.546087004964e+02+4.845363331978e+02i,
       5.546385409189e+02+4.865304269511e+02i,
       5.546148406171e+02+4.883910722336e+02i,
       5.545423607415e+02+4.901273169046e+02i,
       5.544255039624e+02+4.917475857993e+02i,
       5.542683411902e+02+4.932597244941e+02i /),

  vdata_w : [1..6] complex =
    (/ 5.673612178944e+02+5.293246849175e+02i,
       5.631436885271e+02+5.282149986629e+02i,
       5.594024089970e+02+5.270996558037e+02i,
       5.560698047020e+02+5.260027904925e+02i,
       5.530898991250e+02+5.249400845633e+02i,
       5.504159734538e+02+5.239212247086e+02i /),

  vdata_a : [1..6] complex =
    (/ 5.046735008193e+02+5.114047905510e+02i,
       5.059412319734e+02+5.098809666433e+02i,
       5.069376896287e+02+5.098144042213e+02i,
       5.077892868474e+02+5.101336130759e+02i,
       5.085233095391e+02+5.104914655194e+02i,
       5.091487099959e+02+5.107917842803e+02i /),

  vdata_b : [1..20] complex =
    (/ 5.177643571579e+02+5.077803458597e+02i,
       5.154521291263e+02+5.088249431599e+02i,
       5.146409228649e+02+5.096208912659e+02i,
       5.142378756213e+02+5.101023387619e+02i,
       5.139626667737e+02+5.103976610617e+02i,
       5.137423460082e+02+5.105948019802e+02i,
       5.135547056878e+02+5.107404165783e+02i,
       5.133910925466e+02+5.108576573661e+02i,
       5.132470705390e+02+5.109577278523e+02i,
       5.131197729984e+02+5.110460304483e+02i,
       5.130070319283e+02+5.111252433800e+02i,
       5.129070537032e+02+5.111968077718e+02i,
       5.128182883502e+02+5.112616233064e+02i,
       5.127393733383e+02+5.113203605551e+02i,
       5.126691062020e+02+5.113735928093e+02i,
       5.126064276004e+02+5.114218460548e+02i,
       5.125504076570e+02+5.114656139760e+02i,
       5.125002331720e+02+5.115053595966e+02i,
       5.124551951846e+02+5.115415130407e+02i,
       5.124146770029e+02+5.115744692211e+02i /),

  vdata_c : [1..20] complex =
    (/ 5.195078707457e+02+5.149019699238e+02i,
       5.155422171134e+02+5.127578201997e+02i,
       5.144678022222e+02+5.122251847514e+02i,
       5.140150594328e+02+5.121090289018e+02i,
       5.137550426810e+02+5.121143685824e+02i,
       5.135811056728e+02+5.121496764568e+02i,
       5.134569343165e+02+5.121870921893e+02i,
       5.133651975661e+02+5.122193250322e+02i,
       5.132955192805e+02+5.122454735794e+02i,
       5.132410471738e+02+5.122663649603e+02i,
       5.131971141679e+02+5.122830879827e+02i,
       5.131605205716e+02+5.122965869718e+02i,
       5.131290734194e+02+5.123075927445e+02i,
       5.131012720314e+02+5.123166486553e+02i,
       5.130760908195e+02+5.123241541685e+02i,
       5.130528295923e+02+5.123304037599e+02i,
       5.130310107773e+02+5.123356167976e+02i,
       5.130103090133e+02+5.123399592211e+02i,
       5.129905029333e+02+5.123435588985e+02i,
       5.129714421109e+02+5.123465164008e+02i /),

  vdata_d : [1..25] complex =
    (/ 5.122230065252e+02+5.118534037109e+02i,
       5.120463975765e+02+5.117061181082e+02i,
       5.119865766760e+02+5.117096364601e+02i,
       5.119518799488e+02+5.117373863950e+02i,
       5.119269088223e+02+5.117680347632e+02i,
       5.119082416858e+02+5.117967875532e+02i,
       5.118943814638e+02+5.118225281841e+02i,
       5.118842385057e+02+5.118451629348e+02i,
       5.118769435632e+02+5.118649119387e+02i,
       5.118718203448e+02+5.118820803844e+02i,
       5.118683569061e+02+5.118969781011e+02i,
       5.118661708593e+02+5.119098918835e+02i,
       5.118649768950e+02+5.119210777066e+02i,
       5.118645605626e+02+5.119307604484e+02i,
       5.118647586618e+02+5.119391362671e+02i,
       5.118654451572e+02+5.119463757241e+02i,
       5.118665212451e+02+5.119526269238e+02i,
       5.118679083821e+02+5.119580184108e+02i,
       5.118695433664e+02+5.119626617538e+02i,
       5.118713748264e+02+5.119666538138e+02i,
       5.118733606701e+02+5.119700787219e+02i,
       5.118754661974e+02+5.119730095953e+02i,
       5.118776626738e+02+5.119755100241e+02i,
       5.118799262314e+02+5.119776353561e+02i,
       5.118822370068e+02+5.119794338060e+02i /);

function verify() {
  var rerr, ierr : double;
  for iter in 1..niter {
    select problem_class {
      when S {
        rerr = (sums(iter).real - vdata_s(iter).real) / vdata_s(iter).real;
        ierr = (sums(iter).imag - vdata_s(iter).imag) / vdata_s(iter).imag;
      }
      when W {
        rerr = (sums(iter).real - vdata_w(iter).real) / vdata_w(iter).real;
        ierr = (sums(iter).imag - vdata_w(iter).imag) / vdata_w(iter).imag;
      }
      when A {
        rerr = (sums(iter).real - vdata_a(iter).real) / vdata_a(iter).real;
        ierr = (sums(iter).imag - vdata_a(iter).imag) / vdata_a(iter).imag;
      }
      when B {
        rerr = (sums(iter).real - vdata_b(iter).real) / vdata_b(iter).real;
        ierr = (sums(iter).imag - vdata_b(iter).imag) / vdata_b(iter).imag;
      }
      when C {
        rerr = (sums(iter).real - vdata_c(iter).real) / vdata_c(iter).real;
        ierr = (sums(iter).imag - vdata_c(iter).imag) / vdata_c(iter).imag;
      }
      when D {
        rerr = (sums(iter).real - vdata_d(iter).real) / vdata_d(iter).real;
        ierr = (sums(iter).imag - vdata_d(iter).imag) / vdata_d(iter).imag;
      }
    }
    if not (abs(rerr) <= epsilon and abs(ierr) <= epsilon) then
      return false;
  }
  return true;
}

function checksum(iter, X1) {
  var chk = 0;
  [j in 1..1024] chk += X1((5*j) mod nx, (3*j) mod ny, j mod nz);
  chk /= nx * ny * nz;
  sums(i) = chk;
  if verbose then
    writeln("T = ", i, "    Checksum = ", sums(i).real, " ", sums(i).imag);
}

var
  DXYZ : domain(3) = [0..nx-1, 0..ny-1, 0..nz-1],
  U0 : [DXYZ] complex,
  U1 : [DXYZ] complex,
  U2 : [DXYZ] complex,
  Twiddle : [DXYZ] double;

--
-- Run problem once to ensure all data is touched
--

compute_indexmap(Twiddle);
compute_initial_conditions(U1);
fft_init();
fft(1, U1, U0);

--
-- Restart benchmark
--

compute_indexmap(Twiddle);
compute_initial_conditions(U1);
fft_init();
fft(1, U1, U0);

for iter in 1..niter {
  evolve(U0, U1, Twiddle);
  fft(-1, U1, U2);
  checksum(iter, U2);
}

if verify() {
  writeln("Result verification successful.");
} else {
  writeln("Result verification failed.");
}
