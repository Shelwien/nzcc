
#include <intrin.h>

typedef signed char int8;

static inline uint BSR(uint value) {
  unsigned long index;
  _BitScanReverse(&index, value);
  return index;
}

#if 0
static inline uint BSF(uint value) {
  unsigned long index;
  _BitScanForward(&index, value);
  return index;
}
#endif

static void memset32( void* _dst, uint value, size_t n ) {
  size_t i;
  uint* dst = (uint*)_dst;
  for( i=0; i<n; i++ ) dst[i]=value; //*(uint*)((char*)dst + i * 4) = value;
}

static inline int Clamp16 (int v ) {
  return (short(v)!=v) ? (v>>31)^0x7fff : v;
}

const byte cstate[256][2] = {
{  1,  2},{  3,  5},{  4,  6},{  7, 10},{  8, 12},{  9, 13},{ 11, 14},{ 15, 19},
{ 16, 23},{ 17, 24},{ 18, 25},{ 20, 27},{ 21, 28},{ 22, 29},{ 26, 30},{ 31, 33},
{ 32, 35},{ 32, 35},{ 32, 35},{ 32, 35},{ 34, 37},{ 34, 37},{ 34, 37},{ 34, 37},
{ 34, 37},{ 34, 37},{ 36, 39},{ 36, 39},{ 36, 39},{ 36, 39},{ 38, 40},{ 41, 43},
{ 42, 45},{ 42, 45},{ 44, 47},{ 44, 47},{ 46, 49},{ 46, 49},{ 48, 51},{ 48, 51},
{ 50, 52},{ 53, 43},{ 54, 57},{ 54, 57},{ 56, 59},{ 56, 59},{ 58, 61},{ 58, 61},
{ 60, 63},{ 60, 63},{ 62, 65},{ 62, 65},{ 50, 66},{ 67, 55},{ 68, 57},{ 68, 57},
{ 70, 73},{ 70, 73},{ 72, 75},{ 72, 75},{ 74, 77},{ 74, 77},{ 76, 79},{ 76, 79},
{ 62, 81},{ 62, 81},{ 64, 82},{ 83, 69},{ 84, 71},{ 84, 71},{ 86, 73},{ 86, 73},
{ 44, 89},{ 44, 89},{ 88, 91},{ 88, 91},{ 90, 49},{ 90, 49},{ 76, 93},{ 76, 93},
{ 78, 95},{ 78, 95},{ 80, 96},{ 97, 69},{ 98, 87},{ 98, 87},{100, 45},{100, 45},
{ 72, 75},{ 72, 75},{ 74, 77},{ 74, 77},{ 48,103},{ 48,103},{ 92,105},{ 92,105},
{ 80,106},{107, 69},{108, 87},{108, 87},{110, 57},{110, 57},{ 62,113},{ 62,113},
{ 92,115},{ 92,115},{ 80,116},{117, 85},{118, 87},{118, 87},{120, 57},{120, 57},
{ 62,123},{ 62,123},{ 92,125},{ 92,125},{ 94,126},{127, 85},{128,101},{128,101},
{130, 57},{130, 57},{ 62,133},{ 62,133},{102,135},{102,135},{ 94,136},{137, 85},
{138,101},{138,101},{140, 57},{140, 57},{ 62,143},{ 62,143},{102,145},{102,145},
{ 94,146},{147, 99},{148,101},{148,101},{ 68, 57},{ 68, 57},{ 62, 81},{ 62, 81},
{102,149},{102,149},{104,150},{151, 99},{152,111},{112,153},{104,154},{155, 99},
{156,111},{112,157},{104,158},{159, 99},{160,111},{112,161},{104,162},{163,109},
{164,111},{112,165},{114,166},{167,109},{168,121},{122,169},{114,170},{171,109},
{172,121},{122,173},{114,174},{175,109},{176,121},{122,177},{114,178},{179,109},
{180,121},{122,181},{114,182},{183,119},{184,121},{122,185},{124,186},{187,119},
{188,131},{132,189},{124,190},{191,119},{192,131},{132,193},{124,194},{195,119},
{196,131},{132,197},{124,198},{199,119},{200,131},{132,201},{124,202},{203,119},
{204,131},{132,205},{124,206},{207,119},{208,131},{132,209},{124,210},{211,129},
{212,131},{132,213},{134,214},{215,129},{216,141},{142,217},{134,218},{219,129},
{220,141},{142,221},{134,222},{223,129},{224,141},{142,225},{134,226},{227,129},
{228,141},{142,229},{134,230},{231,129},{232,141},{142,233},{134,234},{235,129},
{236,141},{142,237},{134,238},{239,129},{240,141},{142,241},{134,242},{243,129},
{244,141},{142,245},{134,246},{247,139},{248,141},{142,249},{144,250},{251,139},
{252, 69},{ 80,253},{144,254},{251,139},{255, 69},{ 80,253},{144,254},{255, 69},
};


word kModelInterpolation[4096];
word kModelLutLookup[4096];

int ptrim( int p ) {
  p = Min( Max(p,0), 4095 );
  return p;
}

//kModelInterpolation[modele[0].cur >> 20] - 2048;
int stretch1( uint* __restrict p ) {
  return int(kModelInterpolation[p[0]>>20]) - 2048;
}

int stretch( uint p ) {
  return int(kModelInterpolation[p]) - 2048;
}

uint squash( int p ) {
  return kModelLutLookup[ ptrim(p+2048) ];
}

word kLzPredSumLookup[256];
uint kDivideLookup[256];

uint kHashHist8Mult[256];
uint kHashHist4Mult[256];
uint kHashHist10Mult[256];
uint kHashHist24Mult[256];
uint kHashHist6Mult[256];
byte kCmFlagCrap[256];

void CM_InitTables() {
  uint i,a,b;

  for( i=0; i<256; i++ ) {
    kHashHist6Mult[i] =  i * 0x699c6d11;  // 0x2f208239**10
    kHashHist4Mult[i] =  i * 0x502c3f11;  // 0x1000193**4
    kHashHist8Mult[i] =  i * 0x5d615f21;  // 0x1000193**8
    kHashHist10Mult[i] = i * 0xb5199319;  // 0xAE8E8215**10
    kHashHist24Mult[i] = i * 0xd1a55a41;  // 0x99ABC1C7**24
  }

  for( i = 0; i<256; i++) {
    a = 8 * (i >> 4);
    b = 8 * (i & 0xF);
    kCmFlagCrap[i] = 
      2 * (a > 0x32 && b > 0x50 || a > 0x46) | 
      4 * (a <= 0x59 || b <= 0x4F) |
      8 * (a > 0x64 || b > 0x5A) |
     16 * (a <= 0x3F || b <= 0x37) |
     32 * (a <= 0x37 || b <= 0x2F) | 
     64 * (a <= 0x1F && b <= 0x11) |
    128 * (a <= 0x17 && b <= 0xD);
  }

}

void LzCreateTables() {
  uint i;

  kDivideLookup[0] = 255;
  for( i=1; i<255; i++ ) kDivideLookup[i] = 1024 / (3 + i * 2);
  kDivideLookup[255] = 1;

  kLzPredSumLookup[0] = 0;
  for( i=0; i<256; i++ ) kLzPredSumLookup[i] = kModelLutLookup[i * 16 + 8];
  kLzPredSumLookup[255] = 4095;
}

qword SomeMathCrap( qword v, byte shift ) {
  if( v<=1 ) return 0;
  uint bits = BSR((uint)v);
  qword result = qword(bits) << shift;
  uint q = 1 << shift;
  qword m = v << shift >> bits;
  while( q>>=1 ) {
    m = (m * m) >> shift;
    if( m >= (qword(2)<<shift) ) { result+=q; m>>=1; }
  }
  return result;
}

void Build_kModelInterpolation() {
  uint i,j,n,x;

  word* dst = kModelInterpolation;
  dst[0] = 0;
  dst[1] = 0x40;
  for( i=2; i<2048; i++ ) {
    qword v = SomeMathCrap( (4096*i)/(4096-i), 31 );
    dst[i] = (v*170 + 0x43C000000) >> 31;
  }
  for( i=0; i<2048; i++ ) dst[2048+i] = 4096-1 - dst[2048-1-i];

  dst = kModelLutLookup;
  for( i=0,j=0; j<2048; j++ ) {
    x = kModelInterpolation[j];
    for( n=(x-i)>>1; n; n-- ) dst[i++] = j-1;
    while( i<x ) dst[i++] = j;
  }

  for( n=(2048-i)>>1; n; n-- ) dst[i++] = 2048-2;
  while( i<2048 ) dst[i++] = 2048 - 1;

  for( i=0; i<2048; i++ ) dst[2048+i] = 4096-1 - dst[2048-i - 1];
}
