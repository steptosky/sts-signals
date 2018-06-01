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

#include <functional>
#include "gtest/gtest.h"
#include "sts/signals/Signal.h"
#include "Reciever.h"

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

typedef sts::signals::Delegate<const size_t, const size_t> Delegate;

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(Delegate, invalid) {
    auto delegate = Delegate();
    ASSERT_FALSE(delegate.isValid());
}

TEST(Delegate, call_method) {
    Receiver r;
    auto delegate = Delegate::make(&r, &Receiver::slot);
    delegate(10, 20);
    ASSERT_EQ(10, r.mV1);
    ASSERT_EQ(20, r.mV2);
    ASSERT_TRUE(delegate.isValid());
}

TEST(Delegate, call_function) {
    auto delegate = Delegate::make(&Receiver::staticSlot1);
    delegate(10, 20);
    ASSERT_EQ(10, Receiver::g1V1);
    ASSERT_EQ(20, Receiver::g1V2);
    ASSERT_TRUE(delegate.isValid());
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(Delegate, equality_method) {
    Receiver r1;
    Receiver r2;
    ASSERT_TRUE(Delegate::make(&r1, &Receiver::slot) == Delegate::make(&r1, &Receiver::slot));
    ASSERT_FALSE(Delegate::make(&r2, &Receiver::slot) == Delegate::make(&r1, &Receiver::slot));
    ASSERT_FALSE(Delegate::make(&r1, &Receiver::slot) == Delegate::make(&r2, &Receiver::slot));
    ASSERT_FALSE(Delegate::make(&r1, &Receiver::slot) == Delegate::make(&Receiver::staticSlot1));
}

TEST(Delegate, equality_function) {
    ASSERT_TRUE(Delegate::make(&Receiver::staticSlot1) == Delegate::make(&Receiver::staticSlot1));
    ASSERT_FALSE(Delegate::make(&Receiver::staticSlot2) == Delegate::make(&Receiver::staticSlot1));
    ASSERT_FALSE(Delegate::make(&Receiver::staticSlot1) == Delegate::make(&Receiver::staticSlot2));
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
