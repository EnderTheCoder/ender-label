//
// Created by ender on 7/14/23.
//

#ifndef ENDER_LABEL_CONCEPTUTIL_HPP
#define ENDER_LABEL_CONCEPTUTIL_HPP
#include "oatpp/core/Types.hpp"
namespace ender_label::util {
template<typename T>
concept DerivedFromDTO = std::is_base_of<oatpp::DTO, T>::value;
}
#endif //ENDER_LABEL_CONCEPTUTIL_HPP
