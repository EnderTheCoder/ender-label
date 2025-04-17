//
// Created by ender on 12/06/24.
//

#ifndef SWAGGER_HPP
#define SWAGGER_HPP

namespace ender_label::util {
    template<typename T>
    std::string swaggerRequiredFields(const std::string_view msg = "Required fields:") {
        std::ostringstream ss;
        ss << msg << "(";
        for (auto &pair: oatpp::Object<T>::getPropertiesMap()) {
            if (pair.second->info.required) {
                ss << pair.first << ", ";
            }
        }
        ss << ");";
        return ss.str();
    }
}
#endif //SWAGGER_HPP
