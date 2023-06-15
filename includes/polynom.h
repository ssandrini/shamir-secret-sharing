#ifndef POLYNOM_H
#define POLYNOM_H

#include <stdint.h>

#define MODULUS 251
#define MODULAR_ARITHMETIC(x) (((x) % MODULUS + MODULUS) % MODULUS)

typedef struct Polynom {
    uint8_t degree; 
    uint8_t * coefficients; //Ascending degree order
} Polynom;

//Evaluate polynom on given x with modular arithmetic
uint8_t evaluate_polynom_mod(Polynom * polynom, int x);

//Free polynom reserved memory
void polynom_destroy(Polynom * polynom);

//Creates Polynom from an array of size bytes using modular arithmetic.
Polynom * polynom_from_bytes(uint8_t * bytes, int size);

#endif