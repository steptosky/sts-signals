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

#include "Defines.h"
#include <cassert>
#include <memory>
#include "DelegateId.h"

namespace sts {
namespace signals {

    /// @cond private

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

    /*!
     * \details Base class for safe delegates.
     */
    template<typename... Args>
    class ISafeDelegate {
    public:

        ISafeDelegate() = default;
        ISafeDelegate(const ISafeDelegate &) = delete;
        ISafeDelegate & operator=(const ISafeDelegate &) = delete;

        virtual ~ISafeDelegate() = default;

        bool operator ==(const ISafeDelegate & r) { return id() == r.id(); }
        bool operator !=(const ISafeDelegate & r) { return !this->operator==(r); }

        virtual void invoke(Args ...) = 0;
        virtual DelegateId id() const = 0;
        virtual ISafeDelegate<Args ...> * clone() const = 0;

    };

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

    /*!
     * \details Stores static function.
     */
    template<typename... Args>
    class SafeStaticDelegate : public ISafeDelegate<Args ...> {
        typedef void (*Function)(Args ...);
    public:

        explicit SafeStaticDelegate(const Function function)
            : mFunc(function) {
            assert(function);
        }

        virtual ~SafeStaticDelegate() = default;

        void invoke(Args ... args) override {
            mFunc(args ...);
        }

        DelegateId id() const override {
            return DelegateId(0, DelegateId::ptrToId(mFunc));
        }

        ISafeDelegate<Args ...> * clone() const override {
            return new SafeStaticDelegate(mFunc);
        }

    private:
        Function mFunc;
    };

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

    /*! 
     * \details Stores object and its method.
     */
    template<typename TObject, typename... Args>
    class SafeMethodDelegate : public ISafeDelegate<Args ...> {
        typedef void (TObject::*Method)(Args ...);
    public:

        SafeMethodDelegate(TObject * obj, const Method method)
            : mObj(obj),
              mMethod(method) {
            assert(obj);
            assert(method);
        }

        virtual ~SafeMethodDelegate() = default;

        void invoke(Args ... args) override {
            (mObj->*mMethod)(args ...);
        }

        DelegateId id() const override {
            return DelegateId(DelegateId::ptrToId(mObj), DelegateId::ptrToId(mMethod));
        }

        ISafeDelegate<Args...> * clone() const override {
            return new SafeMethodDelegate(mObj, mMethod);;
        }

    private:
        TObject * mObj = nullptr;
        Method mMethod;
    };

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

    class AutoDisconnect;

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

    /*!
     * \details Allows you to store impersonal object, object's method and function and
     *          to call function or object's method.
     * \note This delegate uses polymorphism, so each connection allocates memory in the heap.
     *       It invokes callable object with the virtual methods.
     * \note The delegate doesn't support std::function and lambdas.
     * \code
     * typedef sts::signals::Delegate<> Delegate;
     * SignalReceiver r;
     * const auto delegate = Delegate::make(&r, &SignalReceiver::slot);
     * delegate(); // calls r.slot()
     * \endcode
     */
    template<typename... Args>
    class SafeDelegate {
        typedef std::unique_ptr<ISafeDelegate<Args ...>> CallPtr;
    public:

        /*!
         * \details Creates delegate for object and its method.
         * \note This method makes "A" and "B" id in \link DelegateId \endlink
         *       from the pointer of class and function object.
         * \tparam ObjType
         * \param [in] obj
         * \param [in] method
         * \return delegate.
         */
        template<typename ObjType>
        static SafeDelegate make(ObjType * obj, void (ObjType::*method)(Args ...)) {
            assert(obj);
            assert(method);
            return SafeDelegate(new SafeMethodDelegate<ObjType, Args ...>(obj, method));
        }

        /*!
         * \details Creates delegate for static function.
         * \note This method makes "B" id in \link DelegateId \endlink
         *       from the pointer of function object.
         * \param [in] function
         * \return delegate.
         */
        static SafeDelegate make(void (*function)(Args ...)) {
            assert(function);
            return SafeDelegate(new SafeStaticDelegate<Args ...>(function));
        }

        //-------------------------------------------------------------------------

        SafeDelegate() = default;

        SafeDelegate(const SafeDelegate & r)
            : mCall(r.mCall ? r.mCall->clone() : nullptr),
              mDisconnectObj(r.mDisconnectObj) { }

        SafeDelegate(SafeDelegate && r) NOEXCEPT
            : mCall(r.mCall.release()),
              mDisconnectObj(r.mDisconnectObj) {

            r.mCall.reset();
            r.mDisconnectObj = nullptr;
        }

        ~SafeDelegate() = default;

        SafeDelegate & operator=(const SafeDelegate & r) {
            mCall.reset(r.mCall ? r.mCall->clone() : nullptr);
            mDisconnectObj = r.mDisconnectObj;
            return *this;
        }

        SafeDelegate & operator=(SafeDelegate && r) NOEXCEPT {
            mCall.reset(r.mCall.release());
            mDisconnectObj = r.mDisconnectObj;
            r.mDisconnectObj = nullptr;
            return *this;
        }

        //-------------------------------------------------------------------------

        /*!
         * \details Calls internal callable object.
         */
        void operator()(Args ... args) {
            if (mCall) {
                mCall->invoke(args ...);
            }
        }

        bool operator==(const SafeDelegate & d) const {
            return id() == d.id();
        }

        //------------------------------------------------ -------------------------

        /*!
         * \details Id is created during \link SafeDelegate::make \endlink functions call.
         * \return id of this delegate that is based on input pointers.
         */
        DelegateId id() const { return mCall ? mCall->id() : DelegateId(); }

        bool isValid() const { return id().isValid(); }

        //-------------------------------------------------------------------------

        /*!
         * \details Sets pointer to \link AutoDisconnect \endlink object.
         * \param [in] obj
         */
        void setAutoDisconnectObj(AutoDisconnect * obj) { mDisconnectObj = obj; }

        /*!
         * \return pointer to \link AutoDisconnect \endlink object or nullptr if it wasn't set.
         */
        AutoDisconnect * autoDisconnectObj() const { return mDisconnectObj; }

        //-------------------------------------------------------------------------

    private:

        explicit SafeDelegate(ISafeDelegate<Args ...> * delegate)
            : mCall(delegate) {}

        CallPtr mCall;
        AutoDisconnect * mDisconnectObj = nullptr;

    };

    /**************************************************************************************************/
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**************************************************************************************************/

    /// @endcond
}
}
