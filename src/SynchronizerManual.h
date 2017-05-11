#ifndef _SYNCHRONIZER_MANUAL_
#define _SYNCHRONIZER_MANUAL_

#include "SynchronizerPDSCH.h"

using namespace std;

class SynchronizerManual : public SynchronizerPDSCH {
public:
    SynchronizerManual(unsigned cellId, unsigned txAntennas,
                       unsigned phichNg, size_t chans = 1);

    virtual void start() override;

private:
    void drive(int adjust);

    const unsigned _cellId, _txAntennas, _phichNg;
};
#endif /* _SYNCHRONIZER_MANUAL_ */
