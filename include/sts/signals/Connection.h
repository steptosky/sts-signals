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
#include <algorithm>
#include "Delegate.h"

namespace sts {
namespace signals {

    /// @cond private

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

    /*!
     * \details Represents signal-slot connection.
     */
    class Connection {
    public:

        typedef std::vector<Connection> List;
        typedef Delegate<const Connection&> DisconnectDelegate;

        //---------------------------------------------------------------

        Connection() = default;

        /*!
         * \details Create with delegate id.
         * \param [in] id of delegate which is in the signal list. 
         *             For example: because of this id the connection knows
         *             what the delegate work with.
         */
        explicit Connection(const DelegateId id)
            : mId(id) {}

        /*!
         * \details Create with delegate id and delegate for disconnecting.
         * \param [in] id of delegate which is in the signal list.
         *             For example: because of this id the connection knows
         *             what the delegate work with.
         * \param [in, out] disconnect delegate which is used for disconnecting.
         *                  When you call \link disconnect \endlink
         *                  actually it will call this delegate.
         */
        explicit Connection(const DelegateId id, DisconnectDelegate && disconnect)
            : mId(id),
              mDisconnectDelegate(std::move(disconnect)) {}

        //---------------------------------------------------------------

        /*!
         * \details Checks whether the valid delegate id was set.
         */
        bool isValid() const { return mId.isValid(); }

        DelegateId delegateId() const { return mId; }

        //---------------------------------------------------------------

        bool operator==(const Connection & d) const {
            return mId == d.mId && mDisconnectDelegate == d.mDisconnectDelegate;
        }

        //---------------------------------------------------------------

        /*!
         * \details Calls disconnect delegate if it is set and valid.
         */
        void disconnect() {
            if (isValid()) {
                if (mDisconnectDelegate.isValid()) {
                    mDisconnectDelegate(*this);
                }
            }
        }

        //---------------------------------------------------------------

        /*!
         * \details Adds copy of this connection to the list of 
         *          connections in the auto-disconnector.
         * \details This method works with the auto-disconnector class.
         * \note For internal use only.
         * \tparam T auto-disconnector
         * \param [in, out] connection
         * \param [in, out] p pointer to auto-disconnector class.
         * \todo check ii template can be changed to real class.
         */
        template<typename T>
        static void addToAutoDisconnector(Connection && connection, T * p) {
            if (connection.isValid()) {
                setConnectionToList(std::move(connection), p->mSignalDelegates);
            }
        }

        /*!
         * \details Removes connection as this one from the list of
         *          connections in the auto-disconnector.
         * \details This method works with the auto-disconnector class.
         * \note For internal use only.
         * \tparam T auto-disconnector
         * \param [in] connection
         * \param [in, out] p pointer to auto-disconnector class.
         * \todo check ii template can be changed to real class.
         */
        template<typename T>
        static void removeFromAutoDisconnector(const Connection & connection, T * p) {
            if (connection.isValid()) {
                removeConnectionFromList(connection, p->mSignalDelegates);
            }
        }

    private:

        static void setConnectionToList(Connection && connection, List & p) {
            p.emplace_back(std::move(connection));
        }

        static void removeConnectionFromList(const Connection & connection, List & p) {
            const auto it = std::find_if(p.begin(), p.end(), [&](const Connection & v) {
                return connection == v;
            });
            if (it != p.end()) {
                p.erase(it);
            }
        }

        DelegateId mId;
        DisconnectDelegate mDisconnectDelegate;
    };

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

    /// @endcond

}
}
