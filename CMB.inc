
struct CM_T {
  uint aa[0x2000];
  uint cc[0x40000];
  uint bb[0];
};

struct CM_B {
  uint cmt_mask;
  uint mask;
  uint inverse_mask;
  CM_T* cmt;
  uint* histhash8_ptr;
  uint* histhash4_ptr;
  uint* histhash2_ptr;
  byte* window;
  uint window_size;
  uint write_pos;
  uint hash2pos;
  uint delta1pos;
  uint delta2pos;
  uint hash4pos;
  uint hash8pos;
  short hash2attr;
  short hash4attr;
  word hash8attr;
  word last_bytes;
  word deltaattr;
  byte hash4len;
  byte hash8len;
  byte bestlen;
  byte hash4byte;
  byte hash8byte;
  byte hash2byte;
  byte delta1byte;
  byte delta2byte;
  byte byte4E;
  uint histhash4;
  uint histbyte4;
  byte byte58;
  uint histhash8;
  byte histbyte8[8];
  byte histpos8;

  uint* modelx_ptr;
  uint modelx_cur;

  uint* modely_ptr;
  uint modely_cur;

  word* modelz_ptr;

  byte* modelq_ptr;
  byte modelq_cur;

  word delta_hist_index;
  byte best_delta;

  static const uint modelx_size = 1024;
  uint modelx[modelx_size];

  static const uint modely_size = 1024;
  uint modely[modely_size];

  static const uint modelz_size = 8194;
  word modelz[8194+2];

  static const uint modelq_size = 0x2000;
  byte modelq[modelq_size+3+1];

  word delta_score[255+1];

  byte delta_hist[512];

  word last_pos_for_byte[256];

  #include "CMB_newbyte.inc"
  #include "CMB_proc2.inc"
  #include "CMB_init.inc"
  #include "CMB_finish.inc"
};


