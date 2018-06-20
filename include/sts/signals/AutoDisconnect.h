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

#include <vector>
#include "Defines.h"
#include "Connection.h"

namespace sts {
namespace signals {

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

    /*!
     * \details Inherit from this class when you want to auto-disconnect
     *          the slots while your class is being deleted.
     * \note I recommend you to use this class only if you really need it.
     * \test implement correct copy and move logic and test it.
     */
    class AutoDisconnect {
    public:

        AutoDisconnect() = default;
        AutoDisconnect(const AutoDisconnect &) = default;
        AutoDisconnect & operator=(const AutoDisconnect &) = default;

        STS_SIGNALS_MOVESEM(AutoDisconnect(AutoDisconnect &&) = default;)
        STS_SIGNALS_MOVESEM(AutoDisconnect & operator=(AutoDisconnect &&) = default;)

        virtual ~AutoDisconnect() {
            for (auto & d : mSignalDelegates) {
                d.disconnect();
            }
        }

        /*!
         * \return Number of connected signals.
         */
        size_t numSignalsConnected() const { return mSignalDelegates.size(); }

    private:

        friend Connection;
        std::vector<Connection> mSignalDelegates;

    };

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/
}
}
