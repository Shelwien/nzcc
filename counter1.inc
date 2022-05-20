
#if 1
//  kDivideLookup[0] = 255;
//  for( i=1; i<255; i++ ) kDivideLookup[i] = 1024 / (3 + i * 2);
//  kDivideLookup[255] = 1;
uint C_Update1( uint x, uint bit, uint lim ) {
  byte tt = byte(x);
  uint z = (bit<<23) - (x>>9);
  uint y = kDivideLookup[tt] * z;
  return (y & 0xFFFFFF00) + x + (tt<lim);
}
#else 

union C1 {
  uint X;
  struct {
    uint N    :8;
    uint P    :32-8;
  };
};

uint C_Update1( uint _x, uint bit, uint lim ) {
  C1 x; x.X=_x;
  if( bit==0 ) {
    x.P -= 2*x.P/(x.N*2+3);
  } else {
    x.P += 2*((1<<24)-x.P)/(x.N*2+3);
  }
  x.N += (x.N<lim);
  return x.X;
}
#endif

void C_Update1( uint* __restrict x, uint bit, uint lim ) {
  uint p = x[0];
  p = C_Update1( p, bit, lim );
  x[0] = p;
}

