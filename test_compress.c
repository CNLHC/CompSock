#include "stdio.h"
#include "compress.h"



int main(){
    const char src[]= "Hellllllllllllllllllo, Worrrrrrrld";
    char dst[1<<15];
    compress_buffer(src,25,dst,50);
}
