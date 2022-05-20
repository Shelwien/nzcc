
void CM_B_Finished( uint out_byte ) {
  window[write_pos] = out_byte;
  write_pos = (write_pos + 1) < window_size ? write_pos + 1 : 0;

  last_bytes = (last_bytes << 8) + out_byte;
  histhash2_ptr = &cmt->aa[(last_bytes ^ (last_bytes >> 5)) & 0x1FFF];

  histhash8 += out_byte;
  histhash8 = 0x1000193 * (histhash8 - kHashHist8Mult[histbyte8[histpos8]]);
  histbyte8[histpos8] = out_byte;
  histpos8 = histpos8 < 7 ? histpos8 + 1 : 0;

  histhash8_ptr = &cmt->bb[histhash8 & cmt_mask];

  histhash4 += out_byte;
  histhash4 = 0x1000193 * (histhash4 - kHashHist4Mult[histbyte4 >> 24]);
  histbyte4 = (histbyte4 << 8) | out_byte;
  histhash4_ptr = &cmt->cc[histhash4 & 0x3FFFF];
}

