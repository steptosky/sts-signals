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

#include "gtest/gtest.h"
#include "sts/signals/Signal.h"
#include <functional>
#include "Reciever.h"

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

typedef sts::signals::Signal<const size_t, const size_t> Signal;

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(Signal1, method) {
    Receiver r;
    Signal signal;
    signal.connect(&r, &Receiver::slot);
    signal(10, 20);
    ASSERT_EQ(10, r.mV1);
    ASSERT_EQ(20, r.mV2);
}

TEST(Signal1, duplicate_method) {
    Receiver r;
    Signal signal;
    signal.connect(&r, &Receiver::slot);
    signal.connect(&r, &Receiver::slot);
    ASSERT_EQ(1,signal.slotsCount());
}

TEST(Signal1,_function) {
    Signal signal;
    signal.connect(&Receiver::staticSlot1);
    signal(10, 20);
    ASSERT_EQ(10, Receiver::g1V1);
    ASSERT_EQ(20, Receiver::g1V2);
}

TEST(Signal1, duplicate_function) {
    Signal signal;
    signal.connect(&Receiver::staticSlot1);
    signal.connect(&Receiver::staticSlot1);
    ASSERT_EQ(1, signal.slotsCount());
}

TEST(Signal1, method_and_function) {
    Receiver r;
    Signal signal;
    signal.connect(&r, &Receiver::slot);
    signal.connect(&Receiver::staticSlot1);
    signal(10, 20);
    ASSERT_EQ(10, r.mV1);
    ASSERT_EQ(20, r.mV2);
    ASSERT_EQ(10, Receiver::g1V1);
    ASSERT_EQ(20, Receiver::g1V2);
    ASSERT_EQ(2, signal.slotsCount());
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
