/*************************
 * @file TypeID.h
 * @author Zhile Zhu (zhuzhile08@gmail.com)
 * 
 * @brief Number based TypeID implementation
 * 
 * @date 2024-05-04
 * @copyright Copyright (c) 2024
 *************************/

#pragma once

#include <Common/Common.h>

namespace lyra {

class TypeID {
public:
    template <class> static objectid get() noexcept {
        static const objectid id = ++m_id;
        return id;
    }

private:
    static objectid m_id;
};

objectid TypeID::m_id = 0;

} // namespace lyra
