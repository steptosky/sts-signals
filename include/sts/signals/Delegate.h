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

#include "DelegateId.h"

namespace sts {
namespace signals {

    /********************************************************************************************************/
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /********************************************************************************************************/

    class AutoDisconnect;

    /********************************************************************************************************/
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /********************************************************************************************************/

    /// @cond private

    /*!
     * \details Allows you to store impersonal object, object's method and function and 
     *          to call function or object's method.
     * \note This delegate has "dirty hacks" for storing and calling functions and methods.
     *       In theory those hacks should not lead to problems but it isn't guaranteed.
     *       The std::function can't provide some things that are needed for signals-slots implementation,
     *       that is why this delegate was created.
     * \note The delegate doesn't support std::function and lambdas.
     * \code
     * typedef sts::signals::Delegate<> Delegate;
     * SignalReceiver r;
     * const auto delegate = Delegate::make(&r, &SignalReceiver::slot);
     * delegate(); // calls r.slot()
     * \endcode
     */
    template<typename... Args>
    class Delegate {

        struct Fake {
            void slot(Args ... args) {}
        };

        typedef void (Fake::*FakeMethod)(Args ...);
        typedef void (*FakeFunction)(Args ...);

    public:
        //-------------------------------------------

        /*!
         * \brief 
         * \details Allows you converting a pointer to \link DelegateId::Id \endlink.
         *          So we can use pointer value as impersonal id.
         * \tparam T type pointer
         * \param [in] v 
         * \return id
         */
        template<typename T>
        static DelegateId::Id ptrToId(const T & v) {
            union Convert {
                explicit Convert(const T & v)
                    : mVal(v) {}

                const T mVal;
                const DelegateId::Id mId;
            private:
                // fix warning for the VS 2013, just "=delete" doesn't work properly.
                Convert(const Convert &)
                    : mVal(nullptr) {}

                // fix warning for the VS 2013, just "=delete" doesn't work properly.
                Convert & operator=(const Convert &) { return *this; }
            };
            return Convert(v).mId;
        }

        //-------------------------------------------

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
        static Delegate make(ObjType * obj, void (ObjType::*method)(Args ...)) {
            union Convert {
                explicit Convert(void (ObjType::*method)(Args ...))
                    : mMethod(method) {}

                void (ObjType::*mMethod)(Args ...);
                FakeMethod mFake;
            };
            return Delegate(reinterpret_cast<Fake*>(obj), Convert(method).mFake);
        }

        /*!
         * \details Creates delegate for static function.
         * \note This method makes "B" id in \link DelegateId \endlink
         *       from the pointer of function object.
         * \param [in] function 
         * \return delegate.
         */
        static Delegate make(void (*function)(Args ...)) {
            union Convert {
                explicit Convert(void (*func)(Args ...))
                    : mFunc(func) {}

                void (*mFunc)(Args ...);
                FakeMethod mFake;
            };
            return Delegate(Convert(function).mFake);
        }

        //--------------------------------------------

        Delegate() = default;
        Delegate(const Delegate &) = default;
        Delegate & operator=(const Delegate &) = default;
        ~Delegate() = default;

#if _MSC_VER > 1800 // (2013)
        Delegate(Delegate &&) = default;
        Delegate & operator=(Delegate &&) = default;
#endif
        //--------------------------------------------

        /*!
         * \details Calls internal callable object.
         */
        void operator()(Args ... args) {
            mObj ? (mObj->*mCall.mMethod)(args ...) : mCall.mFunction(args ...);
        }

        bool operator==(const Delegate & d) const {
            return id() == d.id();
        }

        //--------------------------------------------

        /*!
         * \details Id is created during make functions call.
         * \return id of this delegate that is based on input pointers.
         */
        DelegateId id() const {
            if (mObj) {
                return DelegateId(ptrToId<Fake*>(mObj), ptrToId<FakeMethod>(mCall.mMethod));
            }
            return DelegateId(ptrToId<Fake*>(mObj), ptrToId<FakeFunction>(mCall.mFunction));
        }

        bool isValid() const { return id().isValid(); }

        //--------------------------------------------

        /*!
         * \details Sets pointer to \link AutoDisconnect \endlink object.
         * \param [in] obj 
         */
        void setAutoDisconnectObj(AutoDisconnect * obj) { mDisconnectObj = obj; }

        /*!
         * \return pointer to \link AutoDisconnect \endlink object or nullptr if it wasn't set.
         */
        AutoDisconnect * autoDisconnectObj() const { return mDisconnectObj; }

        //--------------------------------------------

    private:

        explicit Delegate(const FakeMethod method)
            : mCall(method) {}

        Delegate(Fake * obj, const FakeMethod method)
            : mObj(obj),
              mCall(method) {}

        // Stores callable objects.
        union Callable {
            explicit Callable()
                : mMethod(nullptr) {}

            explicit Callable(const FakeMethod m)
                : mMethod(m) {}

            explicit Callable(const FakeFunction m)
                : mFunction(m) {}

            FakeFunction mFunction;
            FakeMethod mMethod;
        };

        Fake * mObj = nullptr;
        AutoDisconnect * mDisconnectObj = nullptr;
        Callable mCall;
    };

    /// @endcond

    /********************************************************************************************************/
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /********************************************************************************************************/
}
}
