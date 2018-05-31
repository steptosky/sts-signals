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

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

// Test hierarchy: calling

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

typedef sts::signals::Signal<const size_t> Signal;

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

class AbstractReceiver {
public:

    AbstractReceiver() = default;
    AbstractReceiver(const AbstractReceiver &) = default;
    virtual ~AbstractReceiver() = default;
    AbstractReceiver & operator=(const AbstractReceiver &) = default;

#if _MSC_VER > 1800 // (2013)
    AbstractReceiver(AbstractReceiver &&) = default;
    AbstractReceiver & operator=(AbstractReceiver &&) = default;
#endif

    virtual void slot(const size_t val) { mAbstract = val; }
    size_t mAbstract = 0;

};

class DerivedReceiver : public AbstractReceiver {
public:

    DerivedReceiver() = default;
    DerivedReceiver(const DerivedReceiver &) = default;
    virtual ~DerivedReceiver() = default;
    DerivedReceiver & operator=(const DerivedReceiver &) = default;

#if _MSC_VER > 1800 // (2013)
    DerivedReceiver(DerivedReceiver &&) = default;
    DerivedReceiver & operator=(DerivedReceiver &&) = default;
#endif

    void slot(const size_t val) override { mDerived = val; }
    size_t mDerived = 0;

};

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(Signal3, virtual_slot_case1) {
    Signal signal;
    DerivedReceiver r;
    signal.connect(&r, &DerivedReceiver::slot);
    signal(5);
    ASSERT_EQ(0, r.mAbstract);
    ASSERT_EQ(5, r.mDerived);
}

TEST(Signal3, virtual_slot_case2) {
    Signal signal;
    DerivedReceiver r;
    signal.connect(static_cast<AbstractReceiver*>(&r), &AbstractReceiver::slot);
    signal(5);
    ASSERT_EQ(0, r.mAbstract);
    ASSERT_EQ(5, r.mDerived);
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
