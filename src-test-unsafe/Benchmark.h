#pragma once

/*
**  Copyright(C) 2018, StepToSky
**
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that the following conditions are met:
**
**  1.Redistributions of source code must retain the above copyright notice, this
**    list of conditions and the following disclaimer.
**  2.Redistributions in binary form must reproduce the above copyright notice,
**    this list of conditions and the following disclaimer in the documentation
**    and / or other materials provided with the distribution.
**  3.Neither the name of StepToSky nor the names of its contributors
**    may be used to endorse or promote products derived from this software
**    without specific prior written permission.
**
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**  DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
**  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**  Contacts: www.steptosky.com
*/

#include "sts/signals/Signal.h"
#include "SimpleTimer.h"
#include <iostream>

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

typedef sts::signals::Signal<const size_t, const size_t> Signal;

class SimpleReceiver : public sts::signals::AutoDisconnect {
public:

    SimpleReceiver() = default;
    ~SimpleReceiver() = default;

    void slot(const size_t v1, const size_t v2) {
        mV1 = v1;
        mV2 = v2;
    }

    size_t mV1 = 0;
    size_t mV2 = 0;
};

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

inline void safeBenchmark() {
#ifndef NDEBUG
    const size_t count = 10000;
#else
    const size_t count = 50000;
#endif

    Signal sig;
    SimpleTimer<double> timer;
    std::vector<SimpleReceiver> r(count, SimpleReceiver());
    //-------------------------------------------------------------------------
    std::cout << std::endl;
    std::cout << " =============== Benchmark ==============" << std::endl;
#ifdef STS_SIGNALS_SAFE_DELEGATE
    std::cout << "               safe delegate" << std::endl;
#else
    std::cout << "              unsafe delegate"  << std::endl;
#endif
    std::cout << std::endl;
    std::cout << " Receivers:     " << r.size() << std::endl;
    //-------------------------------------------------------------------------
    timer.start();
    for (auto & v : r) {
        sig.connect(&v, &SimpleReceiver::slot);
    }
    auto tVal = timer.now().toMilliSeconds();
    std::cout << " Connecting:    " << tVal << " ms" << std::endl;
    //-------------------------------------------------------------------------
    timer.start();
    sig(10, 20);
    tVal = timer.now().toMilliSeconds();
    std::cout << " Calling        " << tVal << " ms" << std::endl;
    //-------------------------------------------------------------------------
    timer.start();
    for (auto & v : r) {
        sig.disconnect(&v, &SimpleReceiver::slot);
    }
    tVal = timer.now().toMilliSeconds();
    std::cout << " Disconnecting: " << tVal << " ms" << std::endl;
    //-------------------------------------------------------------------------
    std::cout << " ========================================" << std::endl;
    std::cout << std::endl;
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
