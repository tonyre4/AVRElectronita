#ifdef DEBUGSIGNALS
void testTENS();
void testTENS(){
  currentTENSf=100;
  initTENS();
}

void testGALV();
void testGALV(){ //It will give little pulses
  initGALV();
  DDRD |= 0x60; 
}

void testEXPO();
void testEXPO(){ //It will give little pulses
  initEXPO();
  DDRD |= 0x60; 
}
#endif

