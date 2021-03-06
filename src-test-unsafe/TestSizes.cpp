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

using namespace sts::signals;

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/

TEST(data, sizes) {
    std::cout << "=========================================" << std::endl;
    std::cout << "DelegateId " << sizeof(sts::signals::DelegateId) << std::endl;
    std::cout << "std::function " << sizeof(std::function<void(int, int)>) << std::endl;
    std::cout << "UnsafeDelegate " << sizeof(sts::signals::UnsafeDelegate<int, int>) << std::endl;
    std::cout << "SafeDelegate " << sizeof(sts::signals::SafeDelegate<int, int>) << std::endl;
    std::cout << "SafeMethodDelegate " << sizeof(sts::signals::SafeMethodDelegate<std::vector<std::string>, int, int>) << std::endl;
    std::cout << "SafeStaticDelegate " << sizeof(sts::signals::SafeStaticDelegate<int, int>) << std::endl;
    std::cout << "ISafeDelegate " << sizeof(sts::signals::ISafeDelegate<int, int>) << std::endl;
    std::cout << "Connection " << sizeof(sts::signals::Connection) << std::endl;
    std::cout << "Signal " << sizeof(sts::signals::Signal<int, int>) << std::endl;
    std::cout << "AutoDisconnect " << sizeof(sts::signals::AutoDisconnect) << std::endl;
    std::cout << "=========================================" << std::endl;
}

/**************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////////////
/**************************************************************************************************/
