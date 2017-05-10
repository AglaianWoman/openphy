#ifndef _DECODER_ASN1_
#define _DECODER_ASN1_

#include "DecoderUDP.h"

class DecoderASN1 : public DecoderUDP {
public:
    DecoderASN1() = default;
    DecoderASN1(const DecoderASN1 &d) = default;
    DecoderASN1(DecoderASN1 &&d) = default;
    ~DecoderASN1() = default;

    DecoderASN1 &operator=(const DecoderASN1 &d) = default;
    DecoderASN1 &operator=(DecoderASN1 &&d) = default;

    void send(const char *data, size_t len, uint16_t rnti);
};

#endif /* _DECODER_ASN1_ */
