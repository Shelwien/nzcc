
static void ZeroRp(byte *rp) {
  rp[4] = 0;
  rp[8] = 0;
  rp[12] = 0;
  rp[16] = 0;
  rp[20] = 0;
  rp[24] = 0;
  rp[28] = 0;
  rp[32] = 0;
  rp[36] = 0;
  rp[40] = 0;
  rp[44] = 0;
  rp[48] = 0;
  rp[52] = 0;
  rp[56] = 0;
  rp[60] = 0;
}

static byte *CM_PtrGetX(byte *p, byte b) {
  byte *rp = (byte *)((uintptr_t)&p[64 * (b | 1)] & ~63);
  if( rp[0]==b ) return rp + 4;
  if( rp[1]==b ) return rp + 5;
  if( rp[2]==b ) return rp + 6;
  if( rp[3]==b ) return rp + 7;
  byte v4 = (rp[5] < rp[4]);
  byte v5 = -(rp[6] >= rp[v4 + 4]);
  byte v6 = (v5 & v4) + (~v5 & 2);
  byte v7 = -(rp[7] >= rp[v6 + 4]);
  rp += (v7 & v6) + (~v7 & 3);
  *rp = b;
  ZeroRp(rp);
  return rp + 4;
}

byte* CM_GetHashSlot( uint hash ) {
  CM* t = this;
  byte b = hash >> 24;
  byte *rp = &t->some_ptr[(hash << 6) & t->some_mask];
  if( rp[0]==b ) return rp;
  if (rp[1]==b ) return rp + 1;
  if( rp[2]==b ) return rp + 2;
  if( rp[3]==b ) return rp + 3;
  byte v4 = rp[5] < rp[4];
  byte v5 = -(rp[6] >= rp[v4 + 4]);
  byte v6 = (v5 & v4) + (~v5 & 2);
  byte v7 = -(rp[7] >= rp[v6 + 4]);
  rp += (v7 & v6) + (~v7 & 3);
  *rp = b;
  ZeroRp(rp);
  return rp;
}

