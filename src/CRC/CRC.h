uint8_t reverse8(uint8_t in)
{
  uint8_t x = in;
  x = (((x & 0xAA) >> 1) | ((x & 0x55) << 1));
  x = (((x & 0xCC) >> 2) | ((x & 0x33) << 2));
  x =          ((x >> 4) | (x << 4));
  return x;
}

uint8_t crc8(const uint8_t *array, uint16_t length, const uint8_t polynome, 
             const uint8_t startmask, const uint8_t endmask, 
             const bool reverseIn, const bool reverseOut)
{
  uint8_t crc = startmask;
  while (length--) 
  {
    if ((length & 0xFF) == 0) yield();  // RTOS
    uint8_t data = *array++;
    if (reverseIn) data = reverse8(data);
    crc ^= data;
    for (uint8_t i = 8; i; i--) 
    {
      if (crc & 0x80)
      {
        crc <<= 1;
        crc ^= polynome;
      }
      else
      {
        crc <<= 1;
      }
    }
  }
  crc ^= endmask;
  if (reverseOut) crc = reverse8(crc);
  return crc;
}