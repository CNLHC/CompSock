#include "netinet/tcp.h"

void GenerateSynHeader(struct tcphdr *hdr){
    hdr->syn = 1;
}

void GenerateAckHeader(struct tcphdr *hdr){
    hdr->ack = 1;
}
