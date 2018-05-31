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
#include "Reciever.h"

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

typedef sts::signals::Connection Connection;
typedef sts::signals::Delegate<const size_t, const size_t> Delegate;

/**************************************************************************************************/
//////////////////////////////////////////////////////////////////////////c//////////////////////////
/**************************************************************************************************/

TEST(Connection, make_invalid) {
    const auto deligate = Connection();
    ASSERT_FALSE(deligate.isValid());
}

TEST(Connection, check_id) {
    Receiver r;
    const auto deligate = Delegate::make(&r, &Receiver::slot);
    const auto connection = Connection(deligate.id());
    ASSERT_EQ(deligate.id(), connection.delegateId());
}

TEST(Connection, equlity) {
    Receiver r1;
    Receiver r2;
    const auto connection1 = Connection(Delegate::make(&r1, &Receiver::slot).id());
    const auto connection2 = Connection(Delegate::make(&r2, &Receiver::slot).id());
    ASSERT_TRUE(connection1 == connection1);
    ASSERT_FALSE(connection1 == connection2);
    ASSERT_FALSE(connection2 == connection1);
}

TEST(Connection, disconnect_case1) {
    struct Disc {
        void disconnectByConnection(const Connection &) {
            mCalled = true;
        }

        bool mCalled = false;
    };
    Disc d;
    Receiver r1;
    auto conn = Connection(Delegate::make(&Receiver::staticSlot1).id(),
                           Connection::DisconnectDeligate::make(&d, &Disc::disconnectByConnection));
    conn.disconnect();
    ASSERT_TRUE(d.mCalled);
}

TEST(Connection, disconnect_case2) {
    struct Disc {
        void disconnectByConnection(const Connection &) {
            mCalled = true;
        }

        bool mCalled = false;
    };
    Disc d;
    Receiver r1;
    auto conn = Connection(Delegate::make(&r1, &Receiver::slot).id(),
                           Connection::DisconnectDeligate::make(&d, &Disc::disconnectByConnection));
    conn.disconnect();
    ASSERT_TRUE(d.mCalled);
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
