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

#include <memory>
#include "gtest/gtest.h"
#include "sts/signals/Signal.h"

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

// Test hierarchy: auto-disconnecting

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

typedef sts::signals::Signal<const size_t, const size_t> Signal0;
typedef sts::signals::Signal<const bool> Signal;

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

class Receiver1 : public sts::signals::AutoDisconnect {
public:

    Receiver1() = default;
    Receiver1(const Receiver1 &) = default;
    Receiver1 & operator=(const Receiver1 &) = default;

    virtual ~Receiver1() = default;

    void slotR1(const bool state) { mCalledR1 = state; }
    bool mCalledR1 = false;

};

//-------------------------------------------------------------------------

class Receiver2 : public sts::signals::AutoDisconnect {
public:

    Receiver2() = default;
    Receiver2(const Receiver2 &) = default;
    Receiver2 & operator=(const Receiver2 &) = default;

    virtual ~Receiver2() = default;

    void slotR2(const bool state) { mCalledR2 = state; }
    bool mCalledR2 = false;

};

//-------------------------------------------------------------------------

class Receiver3 : public Receiver1, public Receiver2 {
public:

    Receiver3() = default;
    Receiver3(const Receiver3 &) = default;
    Receiver3 & operator=(const Receiver3 &) = default;

    virtual ~Receiver3() = default;

    void slotR3(const bool state) { mCalledR3 = state; }
    bool mCalledR3 = false;

};

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(AutoDisconnect, multiple_inheritance) {
    Signal sig1;

    // we can't use std::make_unique because it is since c++ 14
    auto r1 = std::unique_ptr<Receiver1>(new Receiver1);
    auto r2 = std::unique_ptr<Receiver2>(new Receiver2);
    auto r3 = std::unique_ptr<Receiver3>(new Receiver3);

    sig1.connect(r1.get(), &Receiver1::slotR1);
    sig1.connect(r2.get(), &Receiver2::slotR2);
    sig1.connect(r3.get(), static_cast<Receiver2 *>(r3.get()), &Receiver3::slotR3);
    sig1.connect(r3.get(), static_cast<Receiver1 *>(r3.get()), &Receiver3::slotR3); // duplicate, not connected actually

    ASSERT_EQ(3, sig1.slotsCount());
    sig1(true);

    ASSERT_TRUE(r1->mCalledR1);
    ASSERT_TRUE(r2->mCalledR2);
    ASSERT_TRUE(r3->mCalledR3);

    r3.reset();
    ASSERT_EQ(2, sig1.slotsCount());
    sig1(false);

    ASSERT_FALSE(r1->mCalledR1);
    ASSERT_FALSE(r2->mCalledR2);

    r1.reset();
    ASSERT_EQ(1, sig1.slotsCount());
    sig1(true);

    ASSERT_TRUE(r2->mCalledR2);

    r2.reset();
    ASSERT_EQ(0, sig1.slotsCount());
}

TEST(AutoDisconnect, signal_deleting) {
    // we can't use std::make_unique because it is since c++ 14
    auto sig1 = std::unique_ptr<Signal>(new Signal);
    auto sig2 = std::unique_ptr<Signal>(new Signal);

    Receiver1 r1;
    Receiver2 r2;
    Receiver3 r3;

    sig1->connect(&r1, &Receiver1::slotR1);
    sig1->connect(&r2, &Receiver2::slotR2);
    sig1->connect(&r3, static_cast<Receiver1 *>(&r3), &Receiver3::slotR3);
    ASSERT_EQ(1, r1.numSignalsConnected());
    ASSERT_EQ(1, r2.numSignalsConnected());
    ASSERT_EQ(1, static_cast<Receiver1 &>(r3).numSignalsConnected());
    (*sig1)(true);
    ASSERT_TRUE(r1.mCalledR1);
    ASSERT_TRUE(r2.mCalledR2);
    ASSERT_TRUE(r3.mCalledR3);

    sig2->connect(&r1, &Receiver1::slotR1);
    sig2->connect(&r2, &Receiver2::slotR2);
    sig2->connect(&r3, static_cast<Receiver1 *>(&r3), &Receiver3::slotR3);
    ASSERT_EQ(2, r1.numSignalsConnected());
    ASSERT_EQ(2, r2.numSignalsConnected());
    ASSERT_EQ(2, static_cast<Receiver1 &>(r3).numSignalsConnected());
    (*sig2)(false);
    ASSERT_FALSE(r1.mCalledR1);
    ASSERT_FALSE(r2.mCalledR2);
    ASSERT_FALSE(r3.mCalledR3);

    sig1.reset();
    ASSERT_EQ(1, r1.numSignalsConnected());
    ASSERT_EQ(1, r2.numSignalsConnected());
    ASSERT_EQ(1, static_cast<Receiver1 &>(r3).numSignalsConnected());
    (*sig2)(true);
    ASSERT_TRUE(r1.mCalledR1);
    ASSERT_TRUE(r2.mCalledR2);
    ASSERT_TRUE(r3.mCalledR3);

    sig2.reset();
    ASSERT_EQ(0, r1.numSignalsConnected());
    ASSERT_EQ(0, r2.numSignalsConnected());
    ASSERT_EQ(0, static_cast<Receiver1 &>(r3).numSignalsConnected());
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
