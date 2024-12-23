#include <stdlib.h>
#include <stdio.h>
#include "mmalloc-fixed.h"
#include <string.h>


int main () {
    for (int i =0 ; i < 100;i++ ) {
        mmalloc(CHUNK_SIZE);
    }
}

