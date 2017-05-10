/*
 * Copyright (C) 2016 Ettus Research LLC
 * Author Tom Tsou <tom.tsou@ettus.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include "DecoderUDP.h"

extern "C" {
#include "lte/log.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
}

using namespace std;

bool DecoderUDP::open(int port)
{
    _sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_sock < 0) {
        ostringstream ostr;
        ostr << "UDP  : Socket creating failed " << _sock;
        LOG_ERR(ostr.str().c_str());
        return false;
    }

    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);

    if (inet_aton("localhost", (struct in_addr *) &_addr.sin_addr) < 0) {
        ostringstream ostr;
        ostr << "UDP  : Socket address conversion failed";
        LOG_ERR(ostr.str().c_str());
        return false;
    }

    return true;
}

void DecoderUDP::send(const char *data, size_t len)
{
    int rc = sendto(_sock, data, len, 0,
                    (const struct sockaddr *) &_addr, sizeof(_addr));
    if (rc < 0) {
        ostringstream ostr;
        ostr << "UDP  : Socket send error " << -rc;
        LOG_ERR(ostr.str().c_str());
    }
}
