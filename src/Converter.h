#ifndef _CONVERTER_H_
#define _CONVERTER_H_

#include <stddef.h>
#include <vector>
#include <complex>

#include "Resampler.h"
#include "SignalVector.h"

template <typename T>
class Converter {
public:
    Converter(size_t chans = 1, size_t taps = 384);
    Converter(const Converter &c) = default;
    Converter(Converter &&c) = default;
    ~Converter() = default;

    Converter &operator=(const Converter &c) = default;
    Converter &operator=(Converter &&c) = default;

    void init(size_t rbs, bool useRadix3);

    void convertPDSCH();
    void convertPBCH();
    void convertPSS();
    void convertPBCH(size_t channel, SignalVector &v);

    void delayPDSCH(std::vector<std::vector<std::complex<float>>> &v, int offset);
    void update();
    void reset();

    auto& raw() { return _buffers; };
    auto& pss() { return _pss; }
    auto& pbch() { return _pbch; }
    auto& pdsch() { return _pdsch; }

private:
    size_t channels() const;
    size_t pdschLen() const;

    std::vector<std::vector<T>> _buffers;

    std::vector<SignalVector> _prev;
    std::vector<SignalVector> _pdsch;
    std::vector<SignalVector> _pbch;
    std::vector<SignalVector> _pss;

    std::vector<Resampler> _pssResamplers;
    std::vector<Resampler> _pbchResamplers;

    bool _convertPDSCH, _convertPBCH, _convertPSS;
    size_t _taps, _rbs;
};

#endif /* _CONVERTER_H_ */
