/*
 * LTE Synchronizer
 *
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

#include "SynchronizerManual.h"

extern "C" {
#include "lte/pbch.h"
#include "lte/slot.h"
#include "lte/si.h"
#include "lte/subframe.h"
#include "lte/ofdm.h"
#include "lte/log.h"
}

/*
 * PDSCH drive sequence without SSS/PBCH recovery
 */
void SynchronizerManual::drive(int adjust)
{
    struct lte_time *time = &_rx->time;

    time->subframe = (time->subframe + 1) % 10;
    if (!time->subframe)
        time->frame = (time->frame + 1) % 1024;

    switch (_rx->state) {
    case LTE_STATE_PSS_SYNC:
        if (syncPSS1() == StatePSS::Found) {
            lte_log_time(time);
            logPSS(_sync.mag, _sync.coarse);
            changeState(LTE_STATE_PSS_SYNC2);
        } else {
            _rx->sync.fine = 9999;
        }
        break;
    case LTE_STATE_PSS_SYNC2:
        if (!time->subframe) {
            lte_log_time(time);
            if (syncPSS2() == StatePSS::Found)
                changeState(LTE_STATE_PDSCH_SYNC);
            else
                changeState(LTE_STATE_PSS_SYNC);
        }
        break;
    case LTE_STATE_PDSCH_SYNC:
        /* SSS must match so we only check timing/frequency on 0 */
        if (time->subframe == 5) {
            if (syncPSS4() == StatePSS::NotFound && _pssMisses > 100) {
                resetState();
                break;
            }
        }
    case LTE_STATE_PDSCH:
        if (timePDSCH(time)) {
            auto lbuf = _inboundQueue->readNoBlock();
            if (!lbuf) {
                LOG_ERR("SYNC  : Dropped frame");
                break;
            }

            handleFreqOffset(lbuf->freqOffset);

            if (lbuf->crcValid) {
                _pssMisses = 0;
                _sssMisses = 0;
                lbuf->crcValid = false;
            }

            lbuf->rbs = _rx->rbs;
            lbuf->cellId = _cellId;
            lbuf->ng = _phichNg;
            lbuf->txAntennas = _txAntennas;
            lbuf->sfn = time->subframe;
            lbuf->fn = time->frame;

            _converter.delayPDSCH(lbuf->buffers, adjust);
            _outboundQueue->write(lbuf);
        }
    }

    _converter.update();
}

/*
 * Manual synchronizer loop 
 */
void SynchronizerManual::start()
{
    _stop = false;
    IOInterface<complex<short>>::start();

    setCellId(_cellId);

    for (int counter = 0;; counter++) {
        int shift = getBuffer(_converter.raw(), counter,
                              _rx->sync.coarse,
                              _rx->sync.fine,
                              _rx->state == LTE_STATE_PDSCH_SYNC);
        _rx->sync.coarse = 0;
        _rx->sync.fine = 0;

        drive(shift);
        _converter.reset();

        if (_reset) resetState();
        if (_stop) break;
    }
}

SynchronizerManual::SynchronizerManual(unsigned cellId, unsigned txAntennas,
                                       unsigned phichNg, size_t chans)
  : SynchronizerPDSCH(chans),
    _cellId(cellId), _txAntennas(txAntennas), _phichNg(phichNg)
{
}
