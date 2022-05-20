
int CM_B_Proc2( int bit_history, int bit, int bitindex, short *factors ) {
  int bitindex_minus1 = bitindex - 1;

  factors[0] = 0;
  if (hash4attr >= 0) {
    uint hash4byte = this->hash4byte + 256, v8;
    if (!hash4len || (hash4byte >> bitindex != bit_history)) {
      if (hash4len) {
        hash4attr = hash4attr & 0xFFFFFE00 | 1;
        hash4len = 0;
        bestlen = hash8len;
      }
      if ((hash4attr & 0xF) == 1 || (hash4byte >> bitindex == bit_history)) {
        v8 = (hash4attr & 0x7FE0) + 2 * (hash4attr & 0xF) + ((hash4byte >> (bitindex - 1)) & 1);
      } else {
        v8 = 480 + ((hash4byte >> (bitindex - 1)) & 1) + 2 * bitindex;
        hash4attr |= 8;
      }
    } else {
      v8 = (hash4attr & 0x7FE0) + ((hash4byte >> (bitindex - 1)) & 1);
    }
    uint *modelx_ptr_new = &modelx[v8];
    *modelx_ptr = (((bit << 23) - (modelx_cur >> 9)) * kDivideLookup[(byte)modelx_cur] & 0xFFFFFF00) + modelx_cur + ((byte)modelx_cur <= 0x27u);
    modelx_ptr = modelx_ptr_new;
    modelx_cur = *modelx_ptr_new;
    factors[0] = kModelInterpolation[modelx_cur >> 20] - 2048;
  }
  

  uint hash8byte = this->hash8byte + 256, v13;
  if (!hash8len) {
    if ( (hash8attr & 0xF) == 1 || (hash8byte >> bitindex == bit_history) ) {
      v13 = (hash8attr & 0x7FE0) + 2 * (hash8attr & 0xF) + ((hash8byte >> bitindex_minus1) & 1);
    } else {
      v13 = 480 + 2 * bitindex + ((hash8byte >> bitindex_minus1) & 1);
      hash8attr |= 8;
    }
  } else {
    if (hash8byte >> bitindex != bit_history) {
      hash8attr = hash8attr & 0xFE00 | 1;
      hash8len = 0;
      bestlen = hash4len;
      if ((hash8attr & 0xF) == 1) {
        v13 = (hash8attr & 0x7FE0) + 2 * (hash8attr & 0xF) + ((hash8byte >> bitindex_minus1) & 1);
      } else {
        v13 = 480 + 2 * bitindex + ((hash8byte >> bitindex_minus1) & 1);
        hash8attr |= 8;
      }
    } else {
      v13 = (hash8attr & 0x7FE0) + ((hash8byte >> bitindex_minus1) & 1);
    }
  }
  uint *modely_ptr_new = &modely[v13];
  *modely_ptr = (((bit << 23) - (modely_cur >> 9)) * kDivideLookup[(byte)modely_cur] & 0xFFFFFF00) + modely_cur + ((byte)modely_cur <= 39);
  modely_ptr = modely_ptr_new;
  modely_cur = *modely_ptr_new;
  factors[1] = kModelInterpolation[modely_cur >> 20] - 2048;

  uint v17 = hash2byte + 256;
  *modelz_ptr += ((bit << 16) + 8u - *modelz_ptr) >> 4;

  if (hash2attr < 0)
    goto LABEL_34;
  if (v17 >> bitindex == bit_history) {
    modelz_ptr = &modelz[((hash2attr & 7) << 7) + (hash2attr & 0x7F80) + ((v17 >> bitindex_minus1) & 1) + (bitindex_minus1 & 0xFFFFFFFE)];
    factors[2] = kModelInterpolation[(uint)*modelz_ptr >> 4] - 2048;
  } else {
    hash2attr = (hash2attr & 0xFFF0) | (Min(hash2attr & 7, 3) << 7) | (bit << 9) | 0x8001;
    bestlen = Max<byte>(hash4len, hash8len);
  LABEL_34:
    if ((hash2attr & 0xF) == 1 || (v17 >> bitindex == bit_history)) {
      modelz_ptr = &modelz[8 * (hash2attr & 0xF) + (hash2attr & 0x7F80) + ((v17 >> bitindex_minus1) & 1) + (bitindex_minus1 & 0xFFFFFFFE)];
      factors[2] = kModelInterpolation[(uint)*modelz_ptr >> 4] - 2048;
    } else {
      modelz_ptr = modelz + 0x2000;
      factors[2] = 0;
      hash2attr |= 8;
    }
  }

  uint v23 = (delta1byte + 256u) >> bitindex_minus1;
  uint v24 = (delta2byte + 256u) >> bitindex_minus1;
  uint modelq_new = 4 * bitindex_minus1 + 2 * (v23 & 1) + deltaattr + (v24 & 1) + (v24 >> 1 == bit_history) * 32 + (v23 >> 1 == bit_history) * 64;

  *modelq_ptr = modelq_cur + (((bit << 8) + 4u - modelq_cur) >> 3);
  modelq_ptr = &modelq[modelq_new];
  modelq_cur = *modelq_ptr;
  factors[3] = kModelInterpolation[16 * modelq_cur] - 2048;
  return bestlen;
}
