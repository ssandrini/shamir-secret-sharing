#include <stdlib.h>
#include "polynom.h"

uint8_t evaluate_polynom(Polynom * polynom, int x) {
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
    Polynom * polynom = (Polynom *)malloc(sizeof(Polynom));
    if(polynom == NULL) {
        return NULL;
    }
    polynom->degree = size - 1;
    polynom->coefficients = (uint8_t*)malloc(size * sizeof(uint8_t)); 
    if(polynom->coefficients == NULL) {
        free(polynom);
        return NULL;
    }

    for (int i = 0; i < size; i++) 
        polynom->coefficients[i] = MODULAR_ARITHMETIC(bytes[i]);
    
    return polynom; 
}

Polynom * lagrange_interpolate(uint8_t * points, uint8_t * shadow_numbers, int size) {
    Polynom* polynomial = (Polynom*) malloc(sizeof(Polynom));
    if(polynomial == NULL) {
        return NULL;
    }
    polynomial->degree = size - 1;
    polynomial->coefficients = (uint8_t*) calloc(size, sizeof(uint8_t));
    if(polynomial->coefficients == NULL) {
        free(polynomial);
        return NULL;
    }

    int k = 0; 
    int yPrimes[size];
    while (k < size) {
        int currentCoefficient = 0;
        int remaining_coefficients = size - k;
        for (int i = 0; i<remaining_coefficients; i++) {
            int y = 0;
            if(k == 0)
                y = shadow_numbers[i];
            else
                y = MODULAR_ARITHMETIC((yPrimes[i] - polynomial->coefficients[k-1]) * inverses[MODULAR_ARITHMETIC(points[i])]);
            yPrimes[i] = y;
            int li = 1; 
            for (int j=0; j<remaining_coefficients; j++) {
                if(i != j)
                    li *= MODULAR_ARITHMETIC(-1*points[j]* inverses[MODULAR_ARITHMETIC(points[i]-points[j])]);
            }
            currentCoefficient += MODULAR_ARITHMETIC(y*li);
        }
        polynomial->coefficients[k++] = (uint8_t) MODULAR_ARITHMETIC(currentCoefficient); 
    }

    return polynomial; 
}

void printPolynom(Polynom * polynomial) {
    printf("Polynomial Degree: %d\n", polynomial->degree);
    printf("Coefficients: ");
    for (int i = polynomial->degree; i >= 0; i--) {
        printf("%d ", polynomial->coefficients[i]);
    }
    printf("\n");
}