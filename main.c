#include <stdlib.h>
#include <stdio.h>
#include "mmalloc.h"
#include <string.h>


int main () {
    char* a  = mmalloc(0x20);
    strcpy(a, "this is cool");
    char* b = mmalloc(0x100);
    strcpy(b, "skaldhkasjdlkdjslkasdj");

    mfree(a);
    mfree(b);
}

