
#include "counter1.inc"

void C_Update0( byte* __restrict x, uint bit ) {
  x[0] = cstate[ x[0] ][bit];
}

void C_Update2( byte* __restrict x, uint bit ) {
  x[0] += ((-bit&256)-x[0]+16)>>5;
}

int iabs( int x ) const {
  x = x<0 ? -x : x;
  return x;
}

enum{
  P_LOG = 12,
  P_SCALE = 1<<P_LOG,
  P_hSCALE = P_SCALE/2,
  P_mSCALE = P_SCALE-1
};

void CM_Update( uint bit ) {
  uint ii, old_flags=flags;

  // update weights
  int fsum, error = P_mSCALE * bit - next_probability;
  for( ii=0; ii<12; ii++ ) {
    modelh_ptr[ii] = Clamp16( modelh_ptr[ii] + ((((error * 16 * factors[ii]) >> 16) + 1) >> 1) );
  }

  // entropy filter
  uint error_abs = iabs(error);
  errorsum += error_abs >> 7;
  errorhist = errorhist*2 + (error_abs>1280);
  if( bit_index_in_byte==7 ) {
    if( cmb_result<=31 ) {
      // enable submodels based on overall prediction errors
      errorsum = Min<byte>(errorsum, 127);
      errorsum_flt1 = (3 * errorsum_flt1 + errorsum) >> 2;
      errorsum_flt2 = (3 * errorsum_flt2 + 2 + errorsum_flt1) >> 2;
      flags = kCmFlagCrap[16*(errorsum_flt1>>3) + (errorsum_flt2>>3)];
    }
    errorsum = 0;
  }

  out_byte_wip = out_byte_wip*2 + bit;
  bit_index_in_byte++;
  if( out_byte_wip>=0x100 ) {
    // byte finished
    out_byte = (out_byte<<8) + byte(out_byte_wip);
    out_byte_wip = 1;
    bit_index_in_byte = 0;

    cmb.CM_B_Finished( byte(out_byte) );
    cmb.CM_B_NewByte();
//    hashes_arr28[0] = 32 * byte(out_byte);
  }

  cmb_result = cmb.CM_B_Proc2( out_byte_wip, bit, 8-bit_index_in_byte, &factors[8] );


  // order1 FSM+APM+stretch
  C_Update1(modele[0].ptr,bit,128);
  C_Update0(ptrs8[0],bit);
  ptrs8[0] = &modelv[ ((out_byte<<8)&0xFF00) + L_LUT[out_byte_wip] ];
  modele[0].ptr = &modele[0].model[*ptrs8[0]];
  factors[0] = stretch1( modele[0].ptr );

  // order2 FSM+APM+stretch
  C_Update1(modele[1].ptr,bit,254);
  C_Update0(ptrs8[1],bit);
  ptrs8[1] = &modelw[ ((out_byte<<8)&0xFFFF00) + L_LUT[out_byte_wip] ];
  ptrs8_cur[1] = *ptrs8[1];
  modele[1].ptr = &modele[1].model[*ptrs8[1]];
  factors[1] = stretch1( modele[1].ptr );

  byte hist_and = ptrs8_cur[1]!=0;
  byte hist_sum = hist_and;

  // hashed order3 FSM+APM+stretch
  C_Update1(modele[2].ptr,bit,254);
  C_Update0(ptrs8[2],bit);
  if( bit_index_in_byte==0 ) {
    hashes_arr28[2] = (out_byte<<8) ^ byte(out_byte) ^ 0xFFFFFF00;
    uint hash2 = 33595399 * hashes_arr28[2] ^ (hashes_arr28[2] >> 11);
    ptrs8[2] = CM_GetHashSlot(hash2) + 4;
  } else if( bit_index_in_byte==4 ) {
    ptrs8[2] = CM_PtrGetX(ptrs8[2], (byte)(out_byte_wip ^ hashes_arr28[2]));
  } else {
    uint incr = (bit + 1) << ((bit_index_in_byte & 3) - 1);
    ptrs8[2] += incr*4;
  }
  ptrs8_cur[2] = *ptrs8[2];
  modele[2].ptr = &modele[2].model[ptrs8_cur[2]];
  factors[2] = stretch1( modele[2].ptr );

  hist_and &= (ptrs8_cur[2]!=0);
  hist_sum += hist_and;

  // hashed order4 FSM+APM+stretch (optional)
  if( old_flags & 0x10 ) {
    C_Update1(modele[3].ptr,bit,255);
    C_Update0(ptrs8[3],bit);
  }
  if( flags & 0x10 ) {
    if( bit_index_in_byte==0 ) {
      hashes_arr28[3] = out_byte;
      uint hash3 = 33595399 * hashes_arr28[3] ^ (hashes_arr28[3] >> 11);
      ptrs8[3] = CM_GetHashSlot(hash3) + 4;
    } else if( bit_index_in_byte==4 ) {
      ptrs8[3] = CM_PtrGetX(ptrs8[3], (byte)(out_byte_wip ^ hashes_arr28[3]));
    } else {
      uint incr = (bit + 1) << ((bit_index_in_byte & 3) - 1);
      ptrs8[3] += incr*4;
    }
    ptrs8_cur[3] = *ptrs8[3];
    modele[3].ptr = &modele[3].model[ptrs8_cur[3]];
    factors[3] = stretch1( modele[3].ptr );
    hist_and &= (ptrs8_cur[3] != 0);
    hist_sum += hist_and;
  } else {
    factors[3] = 0;
  }

  // order6
  if( old_flags & 0x20 ) {
    C_Update1(modele[4].ptr,bit,255);
    C_Update0(ptrs8[4],bit);
  }
  if( bit_index_in_byte==0 ) {
    histhash6 += byte(out_byte);
    histhash6 = 0x2f208239 * (histhash6 - kHashHist6Mult[histbyte6[histpos6]]);
    histbyte6[histpos6] = byte(out_byte);
    histpos6 = histpos6 < 5 ? (histpos6 + 1) : 0;
  }
  if( flags & 0x20 ) {
    if( bit_index_in_byte==0 ) {
      hashes_arr28[4] = histhash6;
      uint hash4 = hashes_arr28[4] ^ _rotr(hashes_arr28[4], 11);
      ptrs8[4] = CM_GetHashSlot(hash4) + 4;
    } else if( bit_index_in_byte==4 ) {
      ptrs8[4] = CM_PtrGetX(ptrs8[4], (byte)(out_byte_wip ^ hashes_arr28[4]));
    } else {
      uint incr = (bit + 1) << ((bit_index_in_byte & 3) - 1);
      ptrs8[4] += incr*4;
    }
    ptrs8_cur[4] = *ptrs8[4];
    modele[4].ptr = &modele[4].model[ptrs8_cur[4]];
    factors[4] = stretch1( modele[4].ptr );
    hist_and &= (ptrs8_cur[4] != 0);
    hist_sum += hist_and;
  } else {
    factors[4] = 0;
  }

  // order10
  if( old_flags & 0x40 ) {
    C_Update1(modele[5].ptr,bit,255);
    C_Update0(ptrs8[5],bit);
  }
  if( bit_index_in_byte==0 ) {
    histhash10 += byte(out_byte);
    histhash10 = 0xAE8E8215 * (histhash10 - kHashHist10Mult[histbyte10[histpos10]]);
    histbyte10[histpos10] = byte(out_byte);
    histpos10 = histpos10 < 9 ? histpos10 + 1 : 0;
  }
  if( flags & 0x40 ) {
    if( bit_index_in_byte==0 ) {
      hashes_arr28[5] = histhash10;
      uint hash5 = histhash10 ^ _rotr(histhash10, 11);
      ptrs8[5] = CM_GetHashSlot(hash5) + 4;
    } else if( bit_index_in_byte==4 ) {
      ptrs8[5] = CM_PtrGetX(ptrs8[5], (byte)(out_byte_wip ^ hashes_arr28[5]));
    } else {
      uint incr = (bit + 1) << ((bit_index_in_byte & 3) - 1);
      ptrs8[5] += incr*4;
    }
    ptrs8_cur[5] = *ptrs8[5];
    modele[5].ptr = &modele[5].model[ptrs8_cur[5]];
    factors[7] = stretch1( modele[5].ptr );
    hist_and &= (ptrs8_cur[5] != 0);
    hist_sum += hist_and;
  } else {
    factors[5] = 0;
  }

  // order24
  if( old_flags & 0x80 ) {
    C_Update1(modele[6].ptr,bit,255);
    C_Update0(ptrs8[6],bit);
  }
  if( bit_index_in_byte==0 ) {
    histhash24 += byte(out_byte);
    histhash24 = 0x99ABC1C7 * (histhash24 - kHashHist24Mult[histbyte24[histpos24]]);
    histbyte24[histpos24] = byte(out_byte);
    histpos24 = histpos24 < 23 ? histpos24 + 1 : 0;
  }
  if (flags & 0x80) {
    if( bit_index_in_byte==0 ) {
      hashes_arr28[6] = histhash24;
      uint hash6 = histhash24 ^ _rotr(histhash24, 11);
      ptrs8[6] = CM_GetHashSlot(hash6) + 4;
    } else if( bit_index_in_byte==4 ) {
      ptrs8[6] = CM_PtrGetX(ptrs8[6], (byte)(out_byte_wip ^ hashes_arr28[6]));
    } else {
      uint incr = (bit + 1) << ((bit_index_in_byte & 3) - 1);
      ptrs8[6] += incr*4;
    }
    ptrs8_cur[6] = *ptrs8[6];
    modele[6].ptr = &modele[6].model[ptrs8_cur[6]];
    factors[6] = stretch1( modele[6].ptr );
    hist_and &= (ptrs8_cur[6] != 0);
    hist_sum += hist_and;
  } else {
    factors[6] = 0;
  }


  // bitmodel - unaligned order17 bit model
  bit_history2 = (2*bit_history2 + bit) & 0x1FFFF;
  if( old_flags & 2 ) {
    C_Update2( modelf_ptr, bit );
  }
  if( flags & 2 ) {
    modelf_ptr = &modelf[bit_history2];
    modelf_cur = *modelf_ptr;
    factors[4] = 16 * modelf_cur - P_hSCALE;
  }


  // wordmodel? parses last english word until length 128 or non-letter
  if( old_flags & 4 ) {
    C_Update1(modele[7].ptr,bit,255);
    C_Update0(ptrs8[7],bit);
  }
  if( flags & 4 ) {
    if( bit_index_in_byte==0 ) {
      bool is_ascii = false;
      byte ascii_lower = byte(out_byte);
      ascii_lower = ((ascii_lower>='A')&(ascii_lower<='Z'))?ascii_lower+'a'-'A':ascii_lower;
      if( (ascii_lower>='a') & (ascii_lower<='z') ) {
        ascii_lower -= 'a'-1;
        is_ascii = 1;
      } else {
        is_ascii = ( (ascii_lower>0x7F) && (cmb.byte4E<=19) );
      }
      if( is_ascii ) {
        uint ascii_hash = 27*hashes_arr28[7] + ascii_lower;
        hashes_arr28[7] = ascii_hash;
        ascii_counter = (ascii_counter | (ascii_lower >> 7) & (ascii_counter == 0)) + 2;
        if( ascii_counter & 0x80 ) flags &= ~4;
        if( ascii_hash < 27*27*27 ) {
          ptrs8[7] = &modelu[60 * (17 * ascii_hash >> 2) + (17 * ascii_hash & 3)];
        } else {
          uint hash7 = 33595399 * ascii_hash ^ (ascii_hash >> 11);
          ptrs8[7] = CM_GetHashSlot(hash7) + 4;
        }
      } else {
        ascii_counter = 0; hashes_arr28[7] = 0; ptrs8[7] = &modelu[0];
      }
    } else if( bit_index_in_byte==4 ) {
      if( hashes_arr28[7]>=27*27*27 ) {
        ptrs8[7] = CM_PtrGetX(ptrs8[7], (byte)(out_byte_wip ^ hashes_arr28[7]));
      } else {
        uint tmp = 17 * hashes_arr28[7] + 1 + (out_byte_wip & 0xF);
        ptrs8[7] = &modelu[60 * (tmp >> 2) + (tmp & 3)];
      }
    } else {
      uint incr = (bit + 1) << ((bit_index_in_byte & 3) - 1);
      ptrs8[7] += incr*4;
    }
    ptrs8_cur[7] = *ptrs8[7];
    modele[7].ptr = &modele[7].model[ptrs8_cur[7]];
    factors[5] = stretch1( modele[7].ptr );
  } else {
    if( bit_index_in_byte==0 ) { ascii_counter=0; hashes_arr28[7]=0; ptrs8[7]=&modelu[0]; }
    factors[5] = 0;
    ptrs8_cur[7] = 0;
  }


  // sparse order1 FSM+APM: xxxxxxxx 00000000; disabled if normal order1 gives better results
  if( (old_flags&0x40)==0 ) {
    factors0_err += iabs( ((bit<<8)-128) - factors0_div16 )/8;
    factors7_err += iabs( ((bit<<8)-128) - factors7_div16 )/8;
    C_Update0(modeld_ptr,bit);
    C_Update1(modela_ptr,bit,255);
  }
  if( (flags & 0x40)==0 ) {
    modeld_ptr = &modeld[ (out_byte&0xFF00) + L_LUT[out_byte_wip] ];
    modela_ptr = &modela[*modeld_ptr];
    factors[7] = (*modela_ptr >> (32-P_LOG)) - P_hSCALE; // simulate stretched form
    factors0_div16 = (factors[0] >> 4);
    factors7_div16 = (factors[7] >> 4);
    if( bit_index_in_byte==0 ) {
      // entropy approximation
      factors0_err_flt = (15*factors0_err_flt + factors0_err + 8)/16;
      factors7_err_flt = (15*factors7_err_flt + factors7_err + 8)/16;
      factors0_err = 0;
      factors7_err = 0;
    }
    if( (hist_sum>2) || (cmb_result>4) || (factors0_err_flt<=factors7_err_flt) || (factors7_err_flt>110) ) factors[7] = 0;
  }


  // masked order3 FSM+APM: xxx00000 xxx00000 xxx00000; only used for high 5 bits
  if( (old_flags&0x40)==0 ) {
    C_Update0(modelb_ptr,bit);
    C_Update1(modelc_ptr,bit,255);
  }
  if( bit_index_in_byte==0 ) upper_3bits_history = (out_byte & 0xE0) + ( (upper_3bits_history<<3) & 0x1FFFF);
  if( (flags & 0x40)==0 ) {
    if( (bit_index_in_byte>4) || (hist_sum>3) || (flags&1) ) {
      modelb_ptr = &modelb[123];
      modelc_ptr = &modelc[*modelb_ptr];
    } else {
      modelb_ptr = &modelb[upper_3bits_history + out_byte_wip];
      modelc_ptr = &modelc[*modelb_ptr];
      factors[6] = stretch1( modelc_ptr );
    }
  }


  // mixer context
  byte cm_bc = (ptrs8_cur[7] && !(ascii_counter & 1)) ? (ascii_counter > 3) + (ascii_counter > 9) + 1 : 0;
  modelh_ptr = modelh[4*(3*hist_sum+(cmb_result>4)+(cmb_result>9)) + cm_bc];
  // mix the prediction
  for( ii=0,fsum=0; ii<12; ii++ ) fsum += factors[ii] * modelh_ptr[ii];
  next_probability = ptrim( (fsum>>16) + P_hSCALE );

#if 1
  // update previous SSE node
  uint gmax = Min<uint>(255, 1<<(hist_sum+(flags&8)+5)) - (hist_sum==4);
  uint adjust = byte(modelg.cur)>gmax ? gmax-byte(modelg.cur) : 0;
  *modelg.cur_ptr = C_Update1(*modelg.cur_ptr,bit,gmax) + adjust;

  // interpolated SSE<13>
  uint hv = (13-1) * next_probability;
  uint modelg_index = ((hist_sum>3)<<11) + ((errorhist&7)<<8) + out_byte_wip;
  uint P = squash( (next_probability&0xFF0)+8-P_hSCALE );
  modelg.cur_ptr = &modelg.base_ptr[modelg_index*13+(hv>>P_LOG)];
  uint sw = (hv & P_mSCALE);
  uint sse_P = sw*(modelg.cur_ptr[1]>>P_LOG) + (P_SCALE-sw)*(modelg.cur_ptr[0]>>P_LOG);
  modelg.cur_ptr += (sw>=P_hSCALE);
  modelg.cur = *modelg.cur_ptr;

  next_probability = (3*(sse_P>>20) + P + 2)/4;
#endif

#if 1
  // adaptive extrapolation
  uint sP = 2 * stretch( next_probability );
  int errx = int(bit*P_SCALE - next_prob_last)>>4;
  // cmc[i].v0 = 0; cmc[i].v1 = 0x800000;
  cmc_cur->v0 += 512 * errx;
  cmc_cur->v1 += cmc_v1_mult * errx;
  cmc_v1_mult = sP;

  cmc_cur = &cmc[32*byte(out_byte) + 2*(2*bit_index_in_byte+(errorhist&1)) + bit];
  next_prob_last = squash( int( (int(cmc_cur->v0)>>16)*512 + (int(cmc_cur->v1)>>16)*sP + 128 ) >> 8 );
  next_probability = (next_probability + 3*next_prob_last + 2)/4;

  next_probability += (next_probability < P_hSCALE);
#endif
}

