#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 5
#define LIMB_SIZE 64
struct bignum {
  // limbs are stored in little endian order.
  // The idea here is that we save a big num as 63 bit chunks and then
  // do math on them one chunk at a time.
  uint64_t limb[SIZE];
  // Number of bits already occupied.
  // For ex if 65 bits then limb[0] is fully occupied and 2 bits of limb 1 are
  // occupied.
  uint32_t bitsize;
};

int find_msb(uint64_t in){
  int count = 0;
  while(in) {
    count++;
    in >>= 1;
  }
  return count;
}
void add(struct bignum *out, struct bignum *src1, struct bignum *src2) {
  uint64_t carry = 0;
  int bits = src1->bitsize > src2->bitsize ? src1->bitsize : src2->bitsize; 
  int i = 0;
  for (; bits > 0; bits -= (LIMB_SIZE - 1), i++) {
    out->limb[i] = src1->limb[i] + src2->limb[i];
    out->limb[i] += carry;
    // Is MSB 1 ?
    if (out->limb[i] & 0x8000000000000000) {
      out->limb[i] &= 0x7fffffffffffffff;
      carry = 1;
    } else {
      carry = 0;
    }
    if(bits - (LIMB_SIZE-1) > 0){
      out->bitsize +=  (LIMB_SIZE  - 1);
    }
  }
  uint64_t last_limb = out->limb[out->bitsize/(LIMB_SIZE - 1)];

  out->bitsize +=  find_msb(out->limb[out->bitsize/(LIMB_SIZE - 1)]);
}

int rightshift(struct bignum *a, uint32_t val) {
  int retval = 0;
  // shift every chunk one chunk at a time.
  for (int i = 0; i < SIZE; i++) {
    uint32_t shiftcount = val;
    while (shiftcount > 0) {
      uint64_t temp = a->limb[i] & 0x1;
      a->limb[i] >>= 1;
      // pass on the shifted value to the lower chunk.
      if (i > 0) {
        a->limb[i - 1] |= temp << (62 - shiftcount + 1);
      }
      shiftcount--;
      retval = temp & 0x1;
      a->bitsize--;
    }
  }
  return retval;
}



void printbignum(struct bignum *a) {
  for (int i = 0; i < 5; i++) {
    printf("%016llX, ", a->limb[i]);
  }
  printf("\n");
}

void printhexbignum(struct bignum *b) {
  uint64_t vals[5];
  uint64_t orig[5] = {b->limb[0], b->limb[1], b->limb[2], b->limb[3],
                      b->limb[4]};
  for (int i = 0; i < 5; i++) {
    vals[i] = orig[i];
    if (i < 4) {
      int k = i + 1;
      uint64_t temp_limb = orig[k];
      while (k > 0) {
        uint64_t temp = temp_limb & 0x1;
        temp_limb >>= 1;
        vals[i] |= (temp << (63 - k + 1));
        k--;
      }
      orig[i + 1] = temp_limb;
    }
  }
  printf("0x");
  for (int i = 4; i >= 0; i--) {
    printf("%016llX", vals[i]);
  }
  printf("\n");
}
// int a = init_from_decimal_string(&a, "34455999999999999999999999999999")
//
void init_from_decimal_string(struct bignum *a, const char *num) {
  char *c = (char *)calloc(strlen(num) + 1, 1);
  unsigned char buf[63*SIZE] = {0};
  if (c == NULL) {
    printf("malloc failed\n");
    exit(1);
  }
  strcpy(c, num);
  char *r = c;
  char *start = r;

  // Try to do high school division here.
  //
  int val = 0;
  while (!(strlen(start) == 1 && *start == '0')) {
    r = start;
    c = start;
    val = 0;
    int singledigit = strlen(start) == 1;
    while (*c != 0) {
      int digit = *c - '0';
      c++;
      val = val * 10 + digit;
      // process val;
      int rem = val % 2;
      // if r is at start don't add zeros.
      if(val/2 == 0 && r == start && !singledigit){
        val = rem;
        continue;

      }
      val = val/2;
      *r = val + '0';
      r++;
      val = rem;
    }

    *r = 0;

    //val can either be 1 or 0 here
    if (val) {
      buf[a->bitsize/8] |= 1 << (a->bitsize % 8);
    }
    a->bitsize += 1;

    printf("quotient:%s bit:%d\n", start, val);

  }
  int bits = 0;
  int limb = 0;
  int index = 0;
  int shiftindex = 0;
  while(bits < a->bitsize){
    uint64_t bit = (buf[index/8] >> (index % 8)) & 0x1;

    if (bit){
      a->limb[limb] |=  bit <<  shiftindex ;
    }
    bits++;
    index++;
    shiftindex++;
    if( bits % (LIMB_SIZE-1) == 0){
      limb++;
      shiftindex = 0;
    }
  }
  free(start);
}

int main() {
  struct bignum num1 = {{0,0,0,0,0}, 0};
  init_from_decimal_string(&num1, "333");
  struct bignum num2 = {{0,0,0,0,0}, 0};
  init_from_decimal_string(&num2, "300");
  struct bignum out = {{0,0,0,0,0}, 0};
  add(&out, &num1, &num2);
  printhexbignum(&out);

  return 0;
}
