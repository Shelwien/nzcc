
struct Coroutine {

  FILE* _f;
  FILE* _g;
  uint f_quit;
  byte* __restrict outptr;
  byte* __restrict outbeg;

  template <typename T> 
  uint coro_call( T* that ) {
    that->do_process();
    return 0;
  }

  uint get( void ) { return getc(_f); }
  void put( uint c ) { putc(c,_g); }

  void coro_init( void ) { f_quit=0; outptr=outbeg=0; }

  void addinp( byte* inp,uint inplen ) {}

  void addout( byte* out,uint outlen ) {}

  void yield( void* p, int value ) {}

};
