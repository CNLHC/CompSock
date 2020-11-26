#include "zlib.h"

int compress_buffer(
        char *src,
        ssize_t src_len,
        char *dest,
        ssize_t dest_len
        ){
    z_stream zInfo={0};
    zInfo.total_in = src_len;
    zInfo.avail_in = src_len;
    zInfo.total_out = zInfo.avail_out = dest_len;
    zInfo.next_in =  (Bytef *)src;
    zInfo.next_out = (Bytef *)dest;
    int nErr,nRet =-1;
    nErr=deflateInit(&zInfo,Z_DEFAULT_COMPRESSION);
    if (nErr == Z_OK){
        nErr=deflate(&zInfo,Z_FINISH);
        if(nErr == Z_STREAM_END){
            nRet = zInfo.total_out;
        }
    }
    deflateEnd(&zInfo);
    return nRet;
}


