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

#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <type_traits>
#include "Connection.h"
#include "AutoDisconnect.h"

namespace sts {
namespace signals {

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

    /*!
     * \details Implementation of fast and simple signals-slots pattern.
     * \details Example:
     * \code
     *
     * class Sender {
     * public:
     *      // declaration of a signal.
     *      sts::signals::Signal<bool, int> mSignal;
     * };
     * 
     * // If you want the slots to be auto-disconnected when your receiver object is being deleted
     * // you just have to inherit your receiver from sts::signals::AutoDisconnect and nothing more.
     * // Auto-disconnect doesn't work for static functions.
     * class Receiver : public sts::signals::AutoDisconnect {
     * public:
     *      static void slotStaticFunction(bool, int){}
     *      void slotMethod(bool, int){}
     * };
     * 
     * Sender senderObj;
     * Receiver receiverObj;
     * 
     * // Connecting slots to the signal.
     * senderObj.mSignal.connect(&Receiver::slotStaticFunction);       // static function
     * senderObj.mSignal.connect(&receiverObj, &Receiver::slotMethod); // class method
     * 
     * // invoke signal.
     * senderObj.mSignal(true, 5);
     * 
     * // Disconnecting slots from the signal
     * sender.mSignal.disconnect(&Receiver::slotStaticFunctiond);      // static function
     * sender.mSignal.disconnect(&receiverObj, &Receiver::slotMethod); // class method
     * // or
     * sender.mSignal.disconnect(&receiverObj);                        // all class methods of given pointer
     *
     * \endcode
     *
     * \note
     *       \li In this library some "dirty hacks" are used to make slots calling as fast as possible.   
     *           Author of those hacks don't exactly know when those hacks can lead to problems, in theory never but it isn't guaranteed. 
     *           For some controversial situations the tests are written.\n
     *           If you define STS_SIGNALS_SAFE_DELEGATE in your project then those hacks will not be used. 
     *           Read the information below about the differences.
     *       \li This implementation isn't thread safe.
     *       \li This implementation doesn't work with return value. Author thinks it isn't necessary,
     *           you can use reference parameter in signal to make needed behavior.
     *       \li This implementation has auto-disconnect implementation for objects with slots, see \link AutoDisconnect \endlink class.
     *       \li This implementation doesn't support std::function and lambdas.
     *  \pre
     *       With the define STS_SIGNALS_SAFE_DELEGATE you can switch the signal to use safe variant of delegate.\n
     *       \li Safe delegate uses polymorphism for storing slots data, so each connection and copy allocates resources on heap.\n
     *           It also uses virtual method to call slot unlike to the unsafe version of the delegate.
     *       \li Unsafe version of delegate has hacks with types that allow you to store slots without allocating resources on heap.\n
     *           It is also able to call slots almost directly and the call can even be inlined.\n
     * \code
     *       // Simple, not very accurate benchmarks in VS 2017. (01.06.2018)
     *       // This tests show you differences between safe and unsafe delegates,
     *       // so it doesn't matter what PC was used for them.
     *       |---------------------------|-------------|-------------|
     *       | Release benchmark VS 2017 |    SAFE     |   UNSAFE    |
     *       |--------------------------:|:-----------:|:-----------:|
     *       | Receivers                 |   50000     |   50000     |
     *       | Connecting                |   14031 ms  |    1021 ms  |
     *       | Call                      |  0.3522 ms  |  0.2885 ms  |
     *       | Disconnecting             |    1430 ms  |    1120 ms  |
     *       |---------------------------|-------------|-------------|
     *       | Memory                    |   ~10.1 MB  |    ~7.1 MB  |
     *       |---------------------------|-------------|-------------|
     *       
     *       |---------------------------|-------------|-------------|
     *       | Debug benchmark VS 2017   |    SAFE     |   UNSAFE    |
     *       |--------------------------:|:-----------:|:-----------:|
     *       | Receivers                 |   10000     |   10000     |
     *       | Connecting                |   11914 ms  |    4298 ms  |
     *       | Call                      |  0.6627 ms  |  0.1163 ms  |
     *       | Disconnecting             |    3619 ms  |      75 ms  |
     *       |---------------------------|-------------|-------------|
     *       | Memory                    |    ~5.9 MB  |    ~4.1 MB  |
     *       |---------------------------|-------------|-------------|
     * \endcode
     * 
     * \tparam Args signal arguments 
     * \code 
     * sts::Signal<> // no arguments
     * sts::Signal<bool, int &, const float> // with arguments
     * \endcode
     */
    template<typename ... Args>
    class Signal {
        typedef Delegate<Args ...> DelegateType;
        typedef std::vector<DelegateType> DelegateList;
    public:

        //---------------------------------------------------------------
        // @{

        Signal() = default;
        Signal(const Signal &) = default;
        Signal & operator=(const Signal &) = default;
        ~Signal() { removeAll(); }

        STS_SIGNALS_MOVESEM(Signal(Signal &&) = default;)
        STS_SIGNALS_MOVESEM(Signal & operator=(Signal &&) = default;)

        // @}
        //---------------------------------------------------------------
        // @{

        /*!
         * \details Connects a static functions.
         * \code
         *     sender.signal.connect(&function);
         * \endcode
         * \param [in] function pointer to static function.
         * \exception std::runtime_error if this method is called during the signal invocation.
         */
        void connect(void (*function)(Args ...)) {
            if (mInvoke) {
                throw std::runtime_error("You can't connect a new static slot during the signal invocation.");
            }
            auto d = DelegateType::make(function);
            if (!std::any_of(mDelegateList.begin(), mDelegateList.end(), [&](const DelegateType & v) { return d == v; })) {
                mDelegateList.emplace_back(std::move(d));
            }
        }

        /*!
         * \details Disconnects the static functions.
         * \code
         *     sender.signal.disconnect(&function);
         * \endcode
         * \param [in] function pointer to static function which must be disconnected.
         * \exception std::runtime_error if this method is called during the signal invocation.
         */
        void disconnect(void (*function)(Args ...)) {
            if (mInvoke) {
                throw std::runtime_error("You can't disconnect static slot during the signal invocation.");
            }
            removeDelegte(DelegateType::make(function));
        }

        // @}
        //---------------------------------------------------------------
        // @{

        /*!
         * \details Connects object method.
         * \code
         *     sender.signal.connect(&receiver, &AppReceiver::method);
         * \endcode
         * \param [in] obj pointer to object.
         * \param [in] method pointer to object method.
         * \exception std::runtime_error if this method is called during the signal invocation.
         */
        template<class ObjType>
        typename std::enable_if<!std::is_base_of<AutoDisconnect, ObjType>::value, void>::type connect(ObjType * obj, void (ObjType::*method)(Args ...)) {
            if (mInvoke) {
                throw std::runtime_error("You can't connect a new slot during the signal invocation.");
            }
            auto d = DelegateType::make(obj, method);
            if (!std::any_of(mDelegateList.begin(), mDelegateList.end(), [&](const DelegateType & v) { return d == v; })) {
                mDelegateList.emplace_back(std::move(d));
            }
        }

        /*!
         * \details Connects object method.
         *          The object is derived from \link AutoDisconnect \endlink.
         * \code
         *     sender.signal.connect(&receiver, &AppReceiver::method);
         * \endcode
         * \param [in] obj pointer to object.
         * \param [in] method pointer to object method.
         * \exception std::runtime_error if this method is called during the signal invocation.
         */
        template<class ObjType>
        typename std::enable_if<std::is_base_of<AutoDisconnect, ObjType>::value, void>::type connect(ObjType * obj, void (ObjType::*method)(Args ...)) {
            connect(obj, static_cast<AutoDisconnect*>(obj), method);
        }

        /*!
         * \details Connects object method with manually specified \link AutoDisconnect \endlink
         * \details Use it when you get some thing like this: 
         *          'static_cast': ambiguous conversions from 'Receiver3 *' to 'AutoDisconnect *'\n
         *          For example it can happen with multiple inheritance.
         *          
         * \code
         * Signal sig1;
         *     
         * class Receiver1 : public AutoDisconnect { ... }         * 
         * class Receiver2 : public AutoDisconnect { ... }
         * class Receiver3 : public Receiver1, public Receiver2 { ... }
         * 
         * // explicit specifying which Receiver should be used to retrieve the AutoDisconnect.
         * sig1.connect(&r3, static_cast<Receiver2 *>(&r3), &Receiver3::slotR3);
         * \endcode
         * 
         * \param [in] obj pointer to object.
         * \param [in] disconnectObj pointer to derived from \link AutoDisconnect \endlink object.
         * \param [in] method pointer to object method.
         * \exception std::runtime_error if this method is called during the signal invocation.
         */
        template<class ObjType>
        void connect(ObjType * obj, AutoDisconnect * disconnectObj, void (ObjType::*method)(Args ...)) {
            if (mInvoke) {
                throw std::runtime_error("You can't connect a new slot during the signal invocation.");
            }
            auto d = DelegateType::make(obj, method);
            if (!std::any_of(mDelegateList.begin(), mDelegateList.end(), [&](const DelegateType & v) { return d == v; })) {
                d.setAutoDisconnectObj(disconnectObj);
                auto connection = autoDisconnectDelegate(d);
                Connection::addToAutoDisconnecter(std::move(connection), disconnectObj);
                mDelegateList.emplace_back(std::move(d));
            }
        }

        /*!
         * \details Disconnects object method.
         * \code
         *     sender.signal.disconnect(&receiver, &AppReceiver::method);
         * \endcode
         * \param [in] obj pointer to object.
         * \param [in] method pointer to object method which should be disconnected.
         * \exception std::runtime_error if this method is called during the signal invocation.
         */
        template<class ObjType>
        void disconnect(ObjType * obj, void (ObjType::*method)(Args ...)) {
            if (mInvoke) {
                throw std::runtime_error("You can't disconnect slot during the signal invocation.");
            }
            removeDelegte(DelegateType::make(obj, method));
        }

        /*!
         * \details Disconnects all methods of specified object.
         * \code
         *     sender.signal.disconnect(&receiver);
         * \endcode
         * \param [in] obj pointer to object whose methods should be disconnected.
         * \exception std::runtime_error if this method is called during the signal invocation.
         */
        template<class ObjType>
        void disconnect(ObjType * obj) {
            if (mInvoke) {
                throw std::runtime_error("You can't disconnect slots during the signal invocation.");
            }
            removeReceiver(obj);
        }

        // @}
        //---------------------------------------------------------------
    private:
        // @{

        /*!
         * \details Disconnects slot by its connection. 
         *          It is called from auto-disconnecter.
         * \param [in] connection
         * \exception std::runtime_error if this method is called during the signal invocation.
         */
        void disconnectByConnection(const Connection & connection) {
            assert(!mInvoke);
            if (mInvoke) {
                throw std::runtime_error("You can't disconnect slot during the signal invocation.");
            }
            if (connection.isValid()) {
                auto it = std::find_if(mDelegateList.begin(), mDelegateList.end(), [&](const DelegateType & d) {
                    return connection.delegateId() == d.id();
                });
                if (it != mDelegateList.end()) {
                    mDelegateList.erase(it);
                }
            }
        }

        // @}
    public:
        //---------------------------------------------------------------
        // @{

        /*!
         * \details Disconnects all.
         */
        void disconnectAll() { removeAll(); }

        /*!
         * \return Connected slots count.
         */
        size_t slotsCount() const { return mDelegateList.size(); }

        /*!
         * \return True if there is no slot connected.
         */
        size_t isEmpty() const { return slotsCount() == 0; }

        // @}
        //---------------------------------------------------------------
        // @{

        void operator()(Args ... args) {
            mInvoke = true;
            for (auto & d : mDelegateList) {
                d(args ...);
            }
            mInvoke = false;
        }

        // @}
        //---------------------------------------------------------------

    private:

        Connection autoDisconnectDelegate(DelegateType & d) {
            return Connection(d.id(), Connection::DisconnectDeligate::make(this, &Signal::disconnectByConnection));
        }

        void removeFromAutoDisconnecter(DelegateType & d) {
            auto * disconnectObg = d.autoDisconnectObj();
            if (disconnectObg) {
                auto connection = autoDisconnectDelegate(d);
                Connection::removeFromAutoDisconnecter(connection, disconnectObg);
            }
        }

        void removeDelegte(const DelegateType & d) {
            auto it = std::find_if(mDelegateList.begin(), mDelegateList.end(), [&](const DelegateType & v) {
                return d == v;
            });
            if (it != mDelegateList.end()) {
                removeFromAutoDisconnecter(*it);
                mDelegateList.erase(it);
            }
        }

        template<class T>
        void removeReceiver(T obj) {
            assert(obj);
            for (auto it = mDelegateList.begin(); it != mDelegateList.end();) {
                if (it->id().mA == DelegateId::ptrToId<T>(obj)) {
                    removeFromAutoDisconnecter(*it);
                    it = mDelegateList.erase(it);
                    if (it == mDelegateList.end()) {
                        break;
                    }
                }
                else {
                    ++it;
                }
            }
        }

        void removeAll() {
            for (auto & d : mDelegateList) {
                removeFromAutoDisconnecter(d);
            }
            mDelegateList.clear();
        }

        DelegateList mDelegateList;
        bool mInvoke = false;

    };

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/
}
}
