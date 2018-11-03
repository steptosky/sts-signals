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

#include <chrono>

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

template<typename Val>
class SimpleTimer {
public:

    SimpleTimer() = default;
    explicit SimpleTimer(bool inStart);

    //-------------------------------------------------------------------------

    void start();
    SimpleTimer & now();

    Val toSeconds();
    Val toMilliseconds();
    Val toMicroSeconds();
    Val toNanoseconds();

    //-------------------------------------------------------------------------

private:

    std::chrono::steady_clock::time_point mStart;
    std::chrono::steady_clock::time_point mEnd;

};

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

template<typename Val>
SimpleTimer<Val>::SimpleTimer(const bool inStart) {
    if (inStart)
        start();
}

template<typename Val>
void SimpleTimer<Val>::start() {
    mStart = std::chrono::steady_clock::now();
}

template<typename Val>
SimpleTimer<Val> & SimpleTimer<Val>::now() {
    mEnd = std::chrono::steady_clock::now();
    return *this;
}

template<typename Val>
Val SimpleTimer<Val>::toSeconds() {
    return std::chrono::duration_cast<std::chrono::duration<Val, std::ratio<1, 1>>>(mEnd - mStart).count();
}

template<typename Val>
Val SimpleTimer<Val>::toMilliseconds() {
    return std::chrono::duration_cast<std::chrono::duration<Val, std::milli>>(mEnd - mStart).count();
}

template<typename Val>
Val SimpleTimer<Val>::toMicroSeconds() {
    return std::chrono::duration_cast<std::chrono::duration<Val, std::micro>>(mEnd - mStart).count();
}

template<typename Val>
Val SimpleTimer<Val>::toNanoseconds() {
    return std::chrono::duration_cast<std::chrono::duration<Val, std::nano>>(mEnd - mStart).count();
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
