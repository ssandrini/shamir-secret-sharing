#include <stdlib.h>
#include "polynom.h"


uint8_t evaluate_polynom_mod(Polynom * polynom, int x) {
    int result = 0;
    
    for (int i = 0; i <= polynom->degree; i++) {
        int term = MODULAR_ARITHMETIC(polynom->coefficients[i]);
        int power = 1;
        for (int j = 0; j < i; j++) {
            power = MODULAR_ARITHMETIC(power * x);
        }
        result = MODULAR_ARITHMETIC(result + MODULAR_ARITHMETIC(term * power));
    }
    
    return (uint8_t) result;
}

void polynom_destroy(Polynom * polynom) {
    free(polynom->coefficients);
    free(polynom);
}

Polynom * polynom_from_bytes(uint8_t * bytes, int size) {
    
    if(size <= 0)
        return NULL;

    Polynom * polynom = (Polynom *)malloc(sizeof(Polynom));
    polynom->degree = size - 1;
    polynom->coefficients = (uint8_t*)malloc(size * sizeof(uint8_t)); 

    for (int i = 0; i < size; i++) 
        polynom->coefficients[i] = MODULAR_ARITHMETIC(bytes[i]);
    
    return polynom; 
}