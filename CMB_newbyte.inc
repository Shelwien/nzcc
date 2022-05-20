
void CM_B_NewByte( void ) {
  byte last_byte = (byte)last_bytes;

  uint last_delta = (word)write_pos - 2 - last_pos_for_byte[last_byte];
  last_pos_for_byte[(byte)last_bytes] = (word)write_pos;

  if (last_delta <= 252) {
    word *dp = &delta_score[delta_hist[delta_hist_index]];
    (*dp)--;
    delta_hist[delta_hist_index] = last_delta;

    dp = &delta_score[last_delta];
    if (*dp >= delta_score[best_delta])
      best_delta = last_delta;
    (*dp)++;
    delta_hist_index = (delta_hist_index + 1) & 0x1FF;
    if (last_delta <= 127 && last_byte == 32) {
      byte4E = (31 * byte4E + 2 * last_delta + 16) >> 5;
    }
  }
  uint histhash2_cur = *histhash2_ptr;
  uint histhash4_cur = *histhash4_ptr;
  uint histhash8_cur = *histhash8_ptr;

  hash2attr += (hash2attr & 7u) < 7;
  hash4attr += (hash4attr & 7u) < 7;
  hash8attr += (hash8attr & 7u) < 7;

  hash4pos = hash4pos + 1 < window_size ? hash4pos + 1 : 0;
  hash8pos = hash8pos + 1 < window_size ? hash8pos + 1 : 0;
  hash2pos = hash2pos + 1 < window_size ? hash2pos + 1 : 0;
  delta1pos = delta1pos + 1 < window_size ? delta1pos + 1 : 0;
  delta2pos = delta2pos + 1 < window_size ? delta2pos + 1 : 0;

  *histhash8_ptr = write_pos | histhash8 & inverse_mask;
  *histhash4_ptr = write_pos | histhash4 & inverse_mask;
  *histhash2_ptr = write_pos | (last_bytes << 16) & inverse_mask;

  if (hash2attr < 0) {
    uint pos = histhash2_cur & mask;
    if ( !(write_pos - 2 - pos > 0xFF ||
         (histhash2_cur ^ (last_bytes << 16)) & inverse_mask ||
         last_bytes >> 8 == (byte)last_bytes)) {
      if ( *(word *)&window[pos - size_t(2)] == *(word *)&window[write_pos - size_t(2)]) {
        hash2pos = pos;
        hash2attr = ((write_pos - pos <= 0x5F) + (write_pos - pos <= 0xF)) << 10;
      }
    }
  }
  word new_deltaattr = deltaattr + (deltaattr < 0x1C00u ? 0x400 : 0);
  if (delta1byte != last_byte) {
    delta1pos = write_pos - (best_delta + 2) < window_size ? write_pos - (best_delta + 2) : 0;
    delta2pos = write_pos - 2 * (best_delta + 2) < window_size ? write_pos - 2 * (best_delta + 2) : 0;
    new_deltaattr = 0;
  } else {
    new_deltaattr = new_deltaattr & 0xFFFFFE7F;
  }
  if (!(best_delta & 1))
    new_deltaattr |= ((write_pos & 3) << 7);
  deltaattr = new_deltaattr;

  if (hash8len) {
    hash8len += (hash8len < 0x3F);
    hash8attr = hash8attr & ~(15<<5) | 32 * Min<uint>(hash8len - 8, 14);
  } else {
    if ( !((histhash8 ^ histhash8_cur) & inverse_mask) ) {
      uint pos = (histhash8_cur & mask);
      if ( *(qword *)&window[pos - size_t(8)] == *(qword *)&window[write_pos - size_t(8)]) {
        if (hash4pos == pos) {
          hash4len = 0;
          hash4attr = -1;
        }
        hash8len = 8;
        hash8pos = pos;
        hash8attr = ((write_pos - pos) < 0x1000) << 9;
      }
    }
  }

  if (hash4len) {
    hash4len += (hash4len < 0x3F);
    hash4attr = hash4attr & ~(15<<5) | 32 * Min<uint>(hash4len - 4, 14);
  } else {
    if ( !((histhash4 ^ histhash4_cur) & inverse_mask) &&
         (*(uint *)&window[(histhash4_cur & mask) - size_t(4)] == *(uint *)&window[write_pos - size_t(4)]) ) {
      hash4len = 4;
      hash4pos = (histhash4_cur & mask);
      hash4attr = ((write_pos - (histhash4_cur & mask)) < 0x1000) << 9;
    }
  }
  bestlen = Max<byte>(hash4len, hash8len);
  hash2byte = window[hash2pos];
  delta1byte = window[delta1pos];
  delta2byte = 2 * window[delta1pos] - window[delta2pos];
  hash4byte = window[hash4pos];
  hash8byte = window[hash8pos];
}

