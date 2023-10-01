


#include <stdio.h>
#include <stdint.h>
#include <math.h>

// Function to calculate (base^exponent) % modulus using square-and-multiply
uint32_t mod_exp(uint32_t base, uint32_t exponent, uint32_t modulus) {
    uint32_t result = 1;
    base %= modulus;

    while (exponent > 0) {
        // If exponent is odd, multiply base with result
        if (exponent % 2 == 1) {
            result = (result * base) % modulus;
        }

        // Square the base and reduce it modulo modulus
        base = (base * base) % modulus;

        // Divide the exponent by 2
        exponent /= 2;
    }

    return result;
}


int main(){

    int px = 18434;
    int d = 1048576;
    int n = 13595;

    printf("%d", mod_exp(px, d, n));




}