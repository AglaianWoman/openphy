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
#include "DecoderASN1.h"

extern "C" {
#include "lte/log.h"
}

#define MAC_LTE_START_STRING_LEN	7
#define MAC_MAX_LEN			512

/* Radio type */
#define FDD_RADIO 1
#define TDD_RADIO 2

/* Direction */
#define DIRECTION_UPLINK   0
#define DIRECTION_DOWNLINK 1

/* RNTI type */
#define NO_RNTI  0
#define P_RNTI   1
#define RA_RNTI  2
#define C_RNTI   3
#define SI_RNTI  4
#define SPS_RNTI 5
#define M_RNTI   6

#define MAC_LTE_START_STRING		"mac-lte"
#define MAC_LTE_RNTI_TAG		0x02
#define MAC_LTE_UEID_TAG		0x03
#define MAC_LTE_SUBFRAME_TAG		0x04
#define MAC_LTE_PREDEFINED_DATA_TAG	0x05
#define MAC_LTE_RETX_TAG		0x06
#define MAC_LTE_CRC_STATUS_TAG		0x07
#define MAC_LTE_EXT_BSR_SIZES_TAG	0x08
#define MAC_LTE_PAYLOAD_TAG		0x01

struct mac_frame {
	char start[MAC_LTE_START_STRING_LEN];
	uint8_t radio_type;
	uint8_t dir;
	uint8_t rnti_type;
	uint8_t rnti_tag;
	uint16_t rnti;
	uint8_t payload_tag;
	char payload[MAC_MAX_LEN];
} __attribute__((packed));

using namespace std;

void DecoderASN1::send(const char *data, size_t len, uint16_t rnti)
{
    struct mac_frame hdr;
    string id(MAC_LTE_START_STRING);

    copy_n(begin(id), MAC_LTE_START_STRING_LEN, hdr.start);
    hdr.radio_type = FDD_RADIO;
    hdr.dir = DIRECTION_DOWNLINK;
    hdr.rnti_type = rnti == 0xffff ? SI_RNTI : RA_RNTI;
    hdr.rnti_tag = MAC_LTE_RNTI_TAG;
    hdr.rnti = htons(rnti);
    hdr.payload_tag = MAC_LTE_PAYLOAD_TAG;
    copy_n(data, len, hdr.payload);

    int rc = sendto(_sock, &hdr, sizeof(hdr) - (MAC_MAX_LEN - len),
                    0, (const struct sockaddr *) &_addr, sizeof(_addr));
    if (rc < 0) {
        ostringstream ostr;
        ostr << "ASN1  : Socket send error " << -rc;
        LOG_ERR(ostr.str().c_str());
    }
}
