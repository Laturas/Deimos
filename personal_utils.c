#ifndef PERUTILS
#define PERUTILS
#include <stdio.h>
#define bool int
#define true 1
#define false 0

/// Swaps the values of two numbers without a temporary variable
#define SWAP(i,j) i = i + j; j = i - j; i = i - j;


#endif