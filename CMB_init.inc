
void CMB_Init( uint b_size, uint winsize ) {
  cmt_mask = b_size - 1;
  cmt = (CM_T*)_aligned_malloc( sizeof(CM_T) + 4*(cmt_mask+1), 16);
  memset( cmt, 0, sizeof(CM_T) + 4*(cmt_mask+1) );

  window_size = winsize;
  mask = (1 << (BSR(winsize-1) + 1)) - 1;
  inverse_mask = ~mask;
  write_pos = 1;
  window = new byte[winsize+64];
  window += 64;
  memset( window-64, 0, window_size+64 );

  memset(last_pos_for_byte, 0, sizeof(last_pos_for_byte));
  best_delta = 0;
  delta_hist_index = 0;
  memset(delta_hist, 0, 0x200u);
  memset(delta_score, 0, 0x1FEu);
  *delta_score = 512;
  last_bytes = 0;
  hash2pos = 0;
  delta2pos = 0;
  hash2attr = 0;
  delta1pos = 0;
  hash8len = 0;
  hash4len = 0;
  hash8pos = 0;
  hash4pos = 0;
  hash8attr = 0;
  hash4attr = 0;
  deltaattr = 0;
  hash4byte = 0;
  hash8byte = 0;
  hash2byte = 0;
  delta1byte = 0;
  delta2byte = 0;
  bestlen = 0;
  byte4E = 0;

  memset32(modelx, 0x80000000, modelx_size);
  modelx_ptr = modelx;
  modelx_cur = *modelx;

  memset32(modely, 0x80000000, modely_size);
  modely_ptr = modely;
  modely_cur = *modely;

  memset32(modelz, 0x80008000, modelz_size>>1);
  modelz_ptr = modelz;

  memset(modelq, 0x80u, 4 * ((modelq_size + 3) >> 2));
  modelq_ptr = modelq;
  modelq_cur = *modelq;

  histhash4 = 0;
  byte58 = 0;
  histbyte4 = 0;
  histhash8 = 0;
  histpos8 = 0;
  memset(histbyte8, 0, sizeof(histbyte8));
}

