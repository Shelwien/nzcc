
struct CM_C {
  uint v0, v1;
};

struct CM_ModelE {
  static const uint size = 256;
  uint model[size];
  uint* ptr;
  uint cur;
};

struct CM_ModelG {
  static const uint size = 53248;
  uint base_ptr[size];
  uint* cur_ptr;
  uint cur;

  void CM_ModelG_Reset( void ) {
    uint i;
    for( i=0; i<13; i++ ) base_ptr[i] = (kModelLutLookup[i * 4096 / 13 + 157] << 20) + 8;
    for( i=13; i<53248; i++ ) base_ptr[i] = base_ptr[i-13];
    cur_ptr = base_ptr;
    cur = *cur_ptr;
  }
};


struct CM {
  byte* some_ptr;
  uint some_mask;
  byte* ptrs8[8];
  uint hashes_arr28[8];
  uint upper_3bits_history;
  uint bit_history2;
  uint out_byte;
  uint next_probability;
  word out_byte_wip;
  uint factors0_div16;
  int8 factors7_div16;
  byte bit_index_in_byte;
  byte cmb_result;
  byte errorhist;
  byte ascii_counter;
  byte flags;
  byte errorsum;
  byte factors7_err_flt;
  byte factors0_err_flt;
  byte errorsum_flt1;
  byte errorsum_flt2;
  byte factors0_err;
  byte factors7_err;
  byte ptrs8_cur[8];
  uint histhash24;
  byte histbyte24[24];
  byte histpos24;
  uint histhash10;
  byte histbyte10[10];
  byte histpos10;
  uint histhash6;
  byte histbyte6[6];
  byte histpos6;

  byte* modeld_ptr;
  byte modeld_cur;

  uint* modela_ptr;
  uint modela_cur;

  byte* modelb_ptr;
  byte modelb_cur;

  uint* modelc_ptr;
  uint modelc_cur;

  byte* modelf_ptr;
  byte modelf_cur;

  CM_C* cmc_cur;
  uint cmc_v0_mult;
  uint cmc_v1_mult;
  uint next_prob_last;

  static const uint modeld_size = 0x10000;
  byte modeld[modeld_size];

  uint modela[256];

  static const uint modelb_size = 0x20000;
  byte modelb[modelb_size];

  uint modelc[256];

  static const uint modelf_size = 0x20000;
  byte modelf[modelf_size];

  CM_ModelE modele[8];

  short factors[12];
  short modelh[84][12];
  short* modelh_ptr;

  CM_B cmb;

  static const uint cmc_size = 0x2000;
  CM_C cmc[cmc_size];

  CM_ModelG modelg;

  byte modelu[0x4CA70C];
  byte modelv[0x10000];
  byte modelw[0x1000000];

  #include "CM_hash.inc"
  #include "CM_init.inc"
  #include "CM_update.inc"
};

