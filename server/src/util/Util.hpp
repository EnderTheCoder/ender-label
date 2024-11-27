//
// Created by ender on 22-10-14.
//

#ifndef ENDER_LABEL_UTIL_HPP
#define ENDER_LABEL_UTIL_HPP

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <memory>
#include "oatpp/core/Types.hpp"
#include "TypeUtil.hpp"

#define ERROR(STATUS, MSG) throw web::protocol::http::HttpError(STATUS, MSG);

namespace ender_label::util{
    class Util {
    public:
        static int setZero() {
            return 0;
        }

        template<class C>
        static auto splitDto(const std::vector<std::shared_ptr<C>> &data_vector) {
            auto result = oatpp::Vector<oatpp::Object<decltype(C::createShared()->getDto())>>::createShared();
            for (const auto &item: data_vector) {
                result->push_back(item->getDto());
            }
            return result;
        }

        template<class C>
        static auto splitPreciseDto(const std::vector<std::shared_ptr<C>> &data_vector) {
            auto result = oatpp::Vector<oatpp::Object<oatpp::DTO>>::createShared();
            for (const auto &item: data_vector) {
                result->push_back(item->getPreciseDto());
            }
            return result;
        }

        template<class C, class DTO>
        static oatpp::Vector<oatpp::Object<DTO>> splitDto(const std::vector<std::shared_ptr<C>> &data_vector) {
            auto result = oatpp::Vector<oatpp::Object<DTO>>::createShared();
            for (const auto &item: data_vector) {
                result->push_back(item->getDto());
            }
            return result;
        }

        template<class C, class DTO>
        static oatpp::Vector<oatpp::Object<DTO>> splitPreciseDto(const std::vector<std::shared_ptr<C>> &data_vector) {
            auto result = oatpp::Vector<oatpp::Object<DTO>>::createShared();
            for (const auto &item: data_vector) {
                result->push_back(item->getPreciseDto());
            }
            return result;
        }

        static bool computeHash(const std::string &str, std::string &result) {
            bool success = false;

            EVP_MD_CTX *context = EVP_MD_CTX_new();

            if (context != nullptr) {
                if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr)) {
                    if (EVP_DigestUpdate(context, str.c_str(), str.length())) {
                        unsigned char hash[64];
                        unsigned int lengthOfHash = 0;

                        if (EVP_DigestFinal_ex(context, hash, &lengthOfHash)) {
                            std::stringstream ss;
                            for (unsigned int i = 0; i < lengthOfHash; ++i) {
                                ss << std::hex << std::setw(2) << std::setfill('0') << (int) hash[i];
                            }

                            result = ss.str();
                            success = true;
                        }
                    }
                }

                EVP_MD_CTX_free(context);
            }

            return success;
        }

        static std::string computeMd5(const std::string &s) {
            unsigned char digest[EVP_MAX_MD_SIZE];
            unsigned int digest_len;
            EVP_MD_CTX *ctx = EVP_MD_CTX_new();
            OATPP_ASSERT_HTTP(ctx != nullptr, Status::CODE_500, "openssl error")
            OATPP_ASSERT_HTTP(EVP_DigestInit_ex(ctx, EVP_md5(), nullptr) == 1, Status::CODE_500, "openssl error")
            OATPP_ASSERT_HTTP(EVP_DigestUpdate(ctx, s.c_str(), s.length()) == 1, Status::CODE_500, "openssl error")
            OATPP_ASSERT_HTTP(EVP_DigestFinal_ex(ctx, digest, &digest_len) == 1, Status::CODE_500, "openssl error")

            EVP_MD_CTX_free(ctx);
            char mdString[33];
            for (unsigned int i = 0; i < digest_len; i++)
                sprintf(&mdString[i * 2], "%02x", (unsigned int) digest[i]);
            return {mdString};
        }

        template<typename LOWER, typename UPPER>
        static auto copyToUpper(LOWER &lower, UPPER &upper) {
            auto lower_map = LOWER::getPropertiesMap();
            auto upper_map = UPPER::getPropertiesMap();
            for (const auto &p : lower_map) {
                if (upper_map.contains(p.first)) {
                    upper_map.at(p.first)->set(upper.get(), p.second->get(lower.get()));
                }
            }
        }
    };

}
#endif //ENDER_LABEL_UTIL_HPP
