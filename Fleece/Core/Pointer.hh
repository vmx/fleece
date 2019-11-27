//
// Pointer.hh
//
// Copyright (c) 2018 Couchbase, Inc All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once
#include "FleeceUsings.hh"
#include "Value.hh"

namespace fleece { namespace impl { namespace internal {

    class Pointer : public Value {
    public:
        static constexpr size_t kMaxNarrowOffset = 0x7FFE;

        Pointer(size_t offset, int width, bool external =false);

        bool isExternal() const                 {return (_byte[0] & 0x40) != 0;}

        // Returns the byte offset
        template <bool WIDE>
        uint32_t offset() const noexcept {
            if (WIDE)
                return (_dec32(wideBytes()) & ~0xC0000000) << 1;
            else
                return (_dec16(narrowBytes()) & ~0xC000) << 1;
        }

        template <bool WIDE>
        inline const Value* deref() const {
            auto off = offset<WIDE>();
            assert(off > 0);
            const Value *dst = offsetby(this, -(ptrdiff_t)off);
            if (_usuallyFalse(isExternal()))
                dst = derefExtern(WIDE, dst);
            return dst;
        }

        const Value* derefWide() const          {return deref<true>();}       // just a workaround

        const Value* deref(bool wide) const {
            return wide ? deref<true>() : deref<false>();
        }

        // assumes data is untrusted, and double-checks offsets for validity.
        const Value* carefulDeref(bool wide,
                                  const void* &dataStart,
                                  const void* &dataEnd) const noexcept;


        bool validate(bool wide, const void *dataStart) const noexcept;

    private:
        // Byte offset as interpreted prior to the 'extern' flag
        template <bool WIDE>
        uint32_t legacyOffset() const noexcept {
            if (WIDE)
                return (_dec32(wideBytes()) & ~0x80000000) << 1;
            else
                return (_dec16(narrowBytes()) & ~0x8000) << 1;
        }

        const Value* derefExtern(bool wide, const Value *dst) const;

        void setNarrowBytes(uint16_t b)             {*(uint16_t*)_byte = b;}
        void setWideBytes(uint32_t b)               {*(uint32_t*)_byte = b;}
        uint16_t narrowBytes() const                {return *(uint16_t*)_byte;}
        uint32_t wideBytes() const                  {return *(uint32_t*)_byte;}
    };


} } }
