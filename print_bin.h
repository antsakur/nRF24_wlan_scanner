#ifndef PRINT_BIN
#define PRINT_BIN

void print_bin(uint8_t bits) {
  char bin_str [8] = {};

  for (int i = 0; i < 8; i++) {
    if ((bits >> i) & 0x1 == 1)
      bin_str[i + 1] = '1';
    else
      bin_str[i + 1] = '0';
  }

  for (int i = 8; i > 0; i--) {
      Serial.write(bin_str[i]);
  }
  
  Serial.println();
}

#endif // PRINT_BIN