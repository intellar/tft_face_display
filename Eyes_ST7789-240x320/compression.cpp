#include "compression.h"


void rle_decode(uint16_t *encodedData, uint16_t encodedLength, uint16_t *decodedData) {
  uint16_t decodedIndex = 0;
  uint16_t i = 0;

  while (i < encodedLength) {
    uint16_t count = encodedData[i++] + 1;
    uint16_t byte = encodedData[i++];

    for (uint16_t j = 0; j < count; j++) {
      decodedData[decodedIndex++] = byte;
    }
  }
}
