
void CM_Init( int a_bits, int b_bits, uint window_size ) {

  some_mask = (1<<a_bits) - 1;
  some_ptr = (byte *)_aligned_malloc(some_mask + 16450, 64);

  cmb.CMB_Init( 1<<b_bits, window_size );

  modeld_ptr = modeld;
  modelb_ptr = modelb;

  ptrs8[0] = modelv;
  ptrs8[1] = modelw;
  byte* vv = CM_GetHashSlot(0);
  ptrs8[2] = vv;
  ptrs8[3] = vv;
  ptrs8[4] = vv;
  ptrs8[5] = vv;
  ptrs8[6] = vv;
  ptrs8[7] = vv;
  for (uint i = 0; i != 8; i++) {
    memset32(modele[i].model, 0x80000000, modele[i].size);
    modele[i].ptr = modele[i].model;
    modele[i].cur = *modele[i].ptr;
  }
  memset(modeld, 0, 4 * ((modeld_size + 3) >> 2));
  modeld_ptr = modeld;
  modeld_cur = *modeld_ptr;

  memset32(modela, 0x80000000, 0x100u);
  modela_ptr = modela;
  modela_cur = modela[0];

  memset(modelb, 0, 4 * ((modelb_size + 3) >> 2));
  modelb_ptr = modelb;
  modelb_cur = *modelb;

  memset32(modelc, 0x80000000, 0x100u);
  modelc_ptr = modelc;
  modelc_cur = modelc[0];

  memset(modelf, 0x80u, 4 * ((modelf_size + 3) >> 2));
  modelf_ptr = modelf;
  modelf_cur = *modelf;

  //CM_ModelG_Reset(&modelg);
  modelg.CM_ModelG_Reset();

  next_prob_last = 0;
  cmc_v1_mult = 0;
  cmc_v0_mult = 0;
  cmc_cur = cmc;
  for(uint i = 0; i < cmc_size; i++) {
    cmc[i].v0 = 0;
    cmc[i].v1 = 0x800000;
  }
  errorsum_flt1 = 127;
  errorsum_flt2 = 127;
  histhash24 = 0;
  histpos24 = 0;
  memset(histbyte24, 0, sizeof(histbyte24));
  histhash10 = 0;
  histpos10 = 0;
  memset(histbyte10, 0, sizeof(histbyte10));
  histhash6 = 0;
  histpos6 = 0;
  memset(histbyte6, 0, sizeof(histbyte6));

  memset(ptrs8_cur, 0, sizeof(ptrs8_cur));
  memset(modelv, 0, 0x10000);
  memset(modelw, 0, 0x1000000);
  memset(modelu, 0, 0x4CA70C);
  memset(hashes_arr28, 0, sizeof(hashes_arr28));
  next_probability = 2048;
  out_byte_wip = 1;
  out_byte = 0;
  bit_index_in_byte = 0;
  errorsum = 0;
  factors7_err_flt = 0;
  factors0_err_flt = 0;
  factors7_div16 = 0;
  upper_3bits_history = 0;
  cmb_result = 0;
  errorhist = 0;

  flags = 0;

  bit_history2 = 0;
  ascii_counter = 0;
  factors7_err = 0;
  factors0_err = 0;
  memset(factors, 0, sizeof(factors));
  memset(modelh, 0, sizeof(modelh));
  modelh_ptr = (short *)modelh;
  memset(some_ptr, 0, some_mask + 16449);

  factors0_div16 = 0;
  factors7_div16 = 0;
}
