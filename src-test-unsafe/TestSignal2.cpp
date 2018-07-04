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

typedef sts::signals::Signal<const std::string&, const int, const int> TestSignal;

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

class AppReceiver : public sts::signals::AutoDisconnect {
public:

    AppReceiver() { resetValues(); }
    AppReceiver(const AppReceiver &) = default;
    AppReceiver & operator=(const AppReceiver &) = default;

    virtual ~AppReceiver() = default;

    void testMethodDelegate(const std::string & inStr, const int inInt1, const int inInt2) {
        m1Str = inStr;
        m1Int1 = inInt1;
        m1Int2 = inInt2;
    }

    void testMethodDelegate2(const std::string & inStr, const int inInt1, const int inInt2) {
        m2Str = inStr;
        m2Int1 = inInt1;
        m2Int2 = inInt2;
    }

    void resetValues() {
        m1Str.clear();
        m1Int1 = 0;
        m1Int2 = 0;

        m2Str.clear();
        m2Int1 = 0;
        m2Int2 = 0;
    }

    std::string m1Str;
    int m1Int1 = 0;
    int m1Int2 = 0;

    std::string m2Str;
    int m2Int1 = 0;
    int m2Int2 = 0;

};

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

class AppStaticReceiver1 {
public:

    static void testStaticDelegate(const std::string & inStr, const int inInt1, const int inInt2) {
        appReceiver.m1Str = inStr;
        appReceiver.m1Int1 = inInt1;
        appReceiver.m1Int2 = inInt2;
    }

    static AppReceiver appReceiver;

};

//-------------------------------------------------------------------------

class AppStaticReceiver2 {
public:

    static void testStaticDelegate(const std::string & inStr, const int inInt1, const int inInt2) {
        appReceiver.m1Str = inStr;
        appReceiver.m1Int1 = inInt1;
        appReceiver.m1Int2 = inInt2;
    }

    static AppReceiver appReceiver;

};

AppReceiver AppStaticReceiver1::appReceiver;
AppReceiver AppStaticReceiver2::appReceiver;

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(Signals2, call_empty) {
    TestSignal sender;
    ASSERT_FALSE(sender.hasConnections());
    sender("Empty test", 0, 0);
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(Signals2, connectiing_disconecting) {
    TestSignal sender1;
    TestSignal sender2;
    AppReceiver receiver1;
    AppReceiver receiver2;

    ASSERT_FALSE(sender1.hasConnections());
    ASSERT_FALSE(sender2.hasConnections());
    sender1.connect(&AppStaticReceiver1::testStaticDelegate);
    sender2.connect(&AppStaticReceiver1::testStaticDelegate);
    ASSERT_EQ(1, sender1.slotsCount());
    ASSERT_EQ(1, sender2.slotsCount());
    sender1.connect(&AppStaticReceiver2::testStaticDelegate);
    sender2.connect(&AppStaticReceiver2::testStaticDelegate);
    ASSERT_EQ(2, sender1.slotsCount());
    ASSERT_EQ(2, sender2.slotsCount());
    sender1.connect(&receiver1, &AppReceiver::testMethodDelegate);
    sender2.connect(&receiver1, &AppReceiver::testMethodDelegate);
    ASSERT_EQ(3, sender1.slotsCount());
    ASSERT_EQ(3, sender2.slotsCount());
    sender1.connect(&receiver2, &AppReceiver::testMethodDelegate);
    sender2.connect(&receiver2, &AppReceiver::testMethodDelegate);
    ASSERT_EQ(4, sender1.slotsCount());
    ASSERT_EQ(4, sender2.slotsCount());

    sender1.disconnect(&AppStaticReceiver1::testStaticDelegate);
    sender2.disconnect(&AppStaticReceiver1::testStaticDelegate);
    ASSERT_EQ(3, sender1.slotsCount());
    ASSERT_EQ(3, sender2.slotsCount());
    sender1.disconnect(&AppStaticReceiver2::testStaticDelegate);
    sender2.disconnect(&AppStaticReceiver2::testStaticDelegate);
    ASSERT_EQ(2, sender1.slotsCount());
    ASSERT_EQ(2, sender2.slotsCount());
    sender1.disconnect(&receiver1, &AppReceiver::testMethodDelegate);
    sender2.disconnect(&receiver1, &AppReceiver::testMethodDelegate);
    ASSERT_EQ(1, sender1.slotsCount());
    ASSERT_EQ(1, sender2.slotsCount());
    sender1.disconnect(&receiver2, &AppReceiver::testMethodDelegate);
    sender2.disconnect(&receiver2, &AppReceiver::testMethodDelegate);
    ASSERT_EQ(0, sender1.slotsCount());
    ASSERT_EQ(0, sender2.slotsCount());

    ASSERT_FALSE(sender1.hasConnections());
    ASSERT_FALSE(sender2.hasConnections());
}

TEST(Signals2, connectiing_disconect_all_of_object) {
    TestSignal sender;
    AppReceiver receiver1;
    AppReceiver receiver2;

    ASSERT_FALSE(sender.hasConnections());
    sender.connect(&receiver1, &AppReceiver::testMethodDelegate);
    sender.connect(&receiver1, &AppReceiver::testMethodDelegate2);
    ASSERT_EQ(2, sender.slotsCount());
    sender.connect(&receiver2, &AppReceiver::testMethodDelegate);
    sender.connect(&receiver2, &AppReceiver::testMethodDelegate2);
    ASSERT_EQ(4, sender.slotsCount());

    sender.disconnect(&receiver1);
    ASSERT_EQ(2, sender.slotsCount());
    sender.disconnect(&receiver2);
    ASSERT_EQ(0, sender.slotsCount());
}

TEST(Signals2, connectiing_disconect_all) {
    TestSignal sender;
    AppReceiver receiver1;
    AppReceiver receiver2;

    ASSERT_FALSE(sender.hasConnections());
    sender.connect(&AppStaticReceiver1::testStaticDelegate);
    ASSERT_EQ(1, sender.slotsCount());
    sender.connect(&AppStaticReceiver2::testStaticDelegate);
    ASSERT_EQ(2, sender.slotsCount());
    sender.connect(&receiver1, &AppReceiver::testMethodDelegate);
    ASSERT_EQ(3, sender.slotsCount());
    sender.connect(&receiver2, &AppReceiver::testMethodDelegate);
    ASSERT_EQ(4, sender.slotsCount());

    sender.disconnectAll();
    ASSERT_EQ(0, sender.slotsCount());
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(Signals2, receiving) {
    TestSignal sender;
    AppReceiver receiver1;
    AppReceiver receiver2;

    sender.connect(&AppStaticReceiver1::testStaticDelegate);
    sender.connect(&AppStaticReceiver2::testStaticDelegate);
    sender.connect(&receiver1, &AppReceiver::testMethodDelegate);
    sender.connect(&receiver2, &AppReceiver::testMethodDelegate);

    sender("signal", 1, 2);
    ASSERT_STREQ("signal", AppStaticReceiver1::appReceiver.m1Str.c_str());
    ASSERT_EQ(1, AppStaticReceiver1::appReceiver.m1Int1);
    ASSERT_EQ(2, AppStaticReceiver1::appReceiver.m1Int2);
    ASSERT_STREQ("signal", AppStaticReceiver2::appReceiver.m1Str.c_str());
    ASSERT_EQ(1, AppStaticReceiver2::appReceiver.m1Int1);
    ASSERT_EQ(2, AppStaticReceiver2::appReceiver.m1Int2);

    ASSERT_STREQ("signal", receiver1.m1Str.c_str());
    ASSERT_EQ(1, receiver1.m1Int1);
    ASSERT_EQ(2, receiver1.m1Int2);
    ASSERT_STREQ("signal", receiver2.m1Str.c_str());
    ASSERT_EQ(1, receiver2.m1Int1);
    ASSERT_EQ(2, receiver2.m1Int2);

    sender.disconnect(&AppStaticReceiver1::testStaticDelegate);
    sender.disconnect(&AppStaticReceiver2::testStaticDelegate);
    sender.disconnect(&receiver1, &AppReceiver::testMethodDelegate);
    sender.disconnect(&receiver2, &AppReceiver::testMethodDelegate);
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
