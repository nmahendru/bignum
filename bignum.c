#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define SIZE 5
struct bignum {
  uint64_t limb[SIZE];
  uint32_t bitsize;
};

void add(struct bignum * out, struct bignum * src1, struct bignum * src2){
  uint64_t carry = 0;
  for (int i = 0; i < SIZE ; i++ ){
    out ->limb[i] = src1->limb[i] + src2->limb[i];
    out->limb[i] += carry;
    if(out->limb[i] & 0x8000000000000000){
      out->limb[i] &= 0x7fffffffffffffff;
      carry = 1;
    } else {
      carry = 0;
    }
  }
}
int rightshift(struct bignum * a, uint32_t val){
  int retval = 0;
  for (int i = 0 ; i < SIZE; i++){
    uint32_t val1 = val;
    while(val > 0){
      uint64_t temp = a->limb[i] & 0x1;
      a->limb[i] >>= 1;
      if(i > 0){
        a->limb[i-1] |= temp << (62 - val + 1);
      }
      val--;
      retval = temp & 0x1;
    }
    val = val1;
  }
  return retval;
}

void printbignum(struct bignum * a){
  for(int i = 0 ; i < 5; i++){
    printf("%016llX, ", a->limb[i]);
  }
  printf("\n");
}

void printhexbignum(struct bignum * b){
  uint64_t vals[5];
  uint64_t orig[5] =  {b->limb[0], b->limb[1], b->limb[2], b->limb[3], b->limb[4]};
  for(int i = 0; i < 5 ; i++){
   vals[i] = orig[i];
   if(i < 4){
     int k = i+1;
     uint64_t temp_limb = orig[k];
     while(k > 0){
      uint64_t temp = temp_limb & 0x1;
      temp_limb >>= 1;
      vals[i] |= (temp << (63 - k + 1));
      k--;
     }
     orig[i+1] = temp_limb;
   }
  }
  printf("0x");
  for(int i = 4 ; i >= 0; i--){
    printf("%016llX", vals[i]);
  }
  printf("\n");
}

int main(){
  struct bignum l = {{0x7fffffffffffffff,0x1,0x1,0,0}};
  struct bignum r = {{0,0x1,0,0,0}};
  printhexbignum(&l);
  printhexbignum(&r);
  struct bignum result;
  add(&result, &r, &l);
  printhexbignum(&result);
  rightshift(&result, 12);
  printhexbignum(&result);
  return 0;
}


