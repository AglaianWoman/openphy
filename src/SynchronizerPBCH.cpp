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

#include "SynchronizerPBCH.h"

extern "C" {
#include "openphy/pbch.h"
#include "openphy/slot.h"
#include "openphy/si.h"
#include "openphy/subframe.h"
#include "openphy/ofdm.h"
#include "openphy/log.h"
}

/*
 * PBCH drive sequence
 */
bool SynchronizerPBCH::drive(int adjust)
{
    struct lte_time *ltime = &_rx->time;
    struct lte_mib mib;
    bool mibValid = false;

    ltime->subframe = (ltime->subframe + 1) % 10;
    if (!ltime->subframe)
        ltime->frame = (ltime->frame + 1) % 1024;

    Synchronizer::drive(ltime, adjust);

    auto logFreq = [](auto freq) {
        ostringstream ost;
        ost << "PBCH  : RF Frequency " << freq / 1e6 << " MHz";
        LOG_CTRL(ost.str().c_str());
    };

    switch (_rx->state) {
    case LTE_STATE_PBCH:
        if (timePBCH(ltime)) {
            if (decodePBCH(ltime, &mib)) {
               _sssMisses = 0;
               _pssMisses = 0;
               _mibDecodeRB = mib.rbs;
               _mibValid = true;
               logFreq(getFreq());
            } else if (++_pssMisses > 10) {
                _reset = true;
            }
        }
        changeState(LTE_STATE_PBCH_SYNC);
    }

    _converter.update();
    return mibValid;
}

/*
 * PBCH synchronizer loop 
 */
void SynchronizerPBCH::start()
{
    _stop = false;
    IOInterface<complex<short>>::start();

    for (int counter = 0;; counter++) {
        int shift = getBuffer(_converter.raw(), counter,
                              _rx->sync.coarse,
                              _rx->sync.fine, 0);
        _rx->sync.coarse = 0;
        _rx->sync.fine = 0;

        if (!_mibValid)
            drive(shift);

        _converter.reset();

        if (_reset) resetState(false);
        if (_stop) break;
   }
}

void SynchronizerPBCH::reset()
{
    _mibValid = false;
    Synchronizer::reset();
}

SynchronizerPBCH::SynchronizerPBCH(size_t chans)
  : Synchronizer(chans), _mibValid(false), _mibDecodeRB(0)
{
}
