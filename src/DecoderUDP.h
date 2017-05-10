#ifndef _DECODER_UDP_
#define _DECODER_UDP_

#include <netinet/in.h>
#include <vector>

class DecoderUDP {
public:
    DecoderUDP() = default;
    DecoderUDP(const DecoderUDP &d) = default;
    DecoderUDP(DecoderUDP &&d) = default;
    virtual ~DecoderUDP() = default;

    DecoderUDP &operator=(const DecoderUDP &d) = default;
    DecoderUDP &operator=(DecoderUDP &&d) = default;

    bool open(int port = 8888);
    void send(const char *data, size_t len);

protected:
    int _sock;
    struct sockaddr_in _addr;
};

#endif /* _DECODER_UDP_ */
