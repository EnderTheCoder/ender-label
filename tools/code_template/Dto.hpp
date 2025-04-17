#ifndef GUARDNAME_HPP
#define GUARDNAME_HPP
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"
#include OATPP_CODEGEN_BEGIN(DTO)

namespace NAMESPACE {
    class DTO_NAME : public oatpp::DTO {
        DTO_INIT(DTO_NAME, DTO)
        DTO_FIELD(Int32, id);
    };
}

#include OATPP_CODEGEN_END(DTO)

#endif //GUARDNAME_HPP