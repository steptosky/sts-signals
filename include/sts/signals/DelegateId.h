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

#include <cstdint>

namespace sts {
namespace signals {

    /// @cond private

    /********************************************************************************************************/
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /********************************************************************************************************/

    /*!
     * \details Represents delegate Id.
     * \details The valid id has at least one parameter as not 0.
     */
    class DelegateId {
    public:

        typedef uintptr_t Id;

        //-------------------------------------------------------------------------

        /*!
         * \details Allows you converting a pointer to \link DelegateId::Id \endlink.
         *          So we can use pointer value as impersonal id.
         * \tparam T type pointer
         * \param [in] v
         * \return id
         */
        template<typename T>
        static Id ptrToId(const T & v) {
            union Convert {
                explicit Convert(const T & v)
                    : mVal(v) {}

                const T mVal;
                const Id mId;
            private:
                // fix warning for the VS 2013, just "=delete" doesn't work properly.
                Convert(const Convert &)
                    : mVal(nullptr) {}

                // fix warning for the VS 2013, just "=delete" doesn't work properly.
                Convert & operator=(const Convert &) { return *this; }
            };
            return Convert(v).mId;
        }

        //-------------------------------------------------------------------------

        explicit DelegateId(const Id a = 0, const Id b = 0)
            : mA(a),
              mB(b) {}

        DelegateId(const DelegateId &) = default;
        ~DelegateId() = default;
        DelegateId & operator=(const DelegateId &) = default;

        //-------------------------------------------------------------------------

        bool operator==(const DelegateId & id) const { return mA == id.mA && mB == id.mB; }
        bool isValid() const { return mA != 0 || mB != 0; }

        //-------------------------------------------------------------------------

        Id mA;
        Id mB;

        //-------------------------------------------------------------------------

    };

    /********************************************************************************************************/
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    /********************************************************************************************************/

    /// @endcond

}
}
