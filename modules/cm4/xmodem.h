#ifndef _XMODE_H_
#define _XMODE_H_

#define XMODE_SOH  0x01
#define XMODE_EOT  0x04
#define XMODE_ACK  0x06
#define XMODE_NAK  0x15
#define XMODE_CAN  0x18
#define XMODE_EOF  0x1A

#define XMODE_MAX_ERROR  20


int ent_recfile(const char *fn);
int ent_senfile(const char *fn);
void XmodemRecData(char *data,int len,int errno);

#endif

