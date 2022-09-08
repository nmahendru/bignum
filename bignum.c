#include <stdint.h>
#include <stdio.h>

struct bignum {
  uint64_t limb[5];
};

void add(struct bignum * dest, struct bignum * src){
  uint64_t carry = 0;
  for (int i = 0; i < sizeof(dest->limb)/ sizeof(dest->limb[0] ) ; i++ ){
    dest->limb[i] += src->limb[i];
    dest->limb[i] += carry;
    if(dest->limb[i] & 0x8000000000000000){
      dest->limb[i] &= 0x7fffffffffffffff;
      carry = 1;
    } else {
      carry = 0;
    }
  }
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
  add(&r, &l);
  printhexbignum(&r);
  return 0;
}


