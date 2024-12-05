//
// Created by SURFACE on 2024/1/14.
//

#ifndef ENDER_LABEL_SERVICEBEAN_HPP
#define ENDER_LABEL_SERVICEBEAN_HPP

#include <ranges>

#include "boost/format.hpp"
#include "dto/request/PagedRequestDto.hpp"
#include "dto/data/CountDto.hpp"
#include "variant"
#include "sstream"

namespace ender_label::service {
    using namespace db;
    using namespace dto;
    using namespace oatpp;

    template<const char *TABLE_NAME, typename DTO_TYPE, typename ID_TYPE=Int32>
    class ServiceBean {
        Object<DTO_TYPE> dto{};

    protected:
        OATPP_COMPONENT(std::shared_ptr<PgDb>, db);

        static auto checkPage(const Int32 &page_size, const Int32 &page_num, const int max_page_size) {
            using namespace web::protocol::http;
            if (page_size <= 0 || page_num <= 0) {
                throw HttpError(Status::CODE_400, "Invalid page arguments.");
            }
            if (page_size > max_page_size) {
                throw HttpError(Status::CODE_400, "Page size too large.");
            }
        }

    public:
        ServiceBean() = default;

        virtual ~ServiceBean() = default;

        explicit ServiceBean(const Object<DTO_TYPE> &dto) {
            this->dto = dto;
        }

        template<typename T=ServiceBean>
        static std::shared_ptr<T> createShared(const Object<DTO_TYPE> &dto) {
            return std::static_pointer_cast<T>(std::make_shared<ServiceBean>(dto));
        }

        static auto getFieldStr() {
            std::ostringstream fields;
            for (const auto &item: DTO_TYPE::Wrapper::getPropertiesMap()) {
                fields << "\"" << item.first << "\", ";
            }
            const auto str = fields.str();
            return str.substr(0, str.length() - 2);
        }

        auto getId() {
            return this->dto->id;
        }

        virtual oatpp::Object<DTO_TYPE> getDto() {
            return this->dto;
        }

        virtual void write() {
            using namespace web::protocol::http;

            std::ostringstream sql;
            std::ostringstream sql_params;
            std::ostringstream sql_values;
            std::unordered_map<String, Void> params;

            for (auto property = this->dto.getPropertiesList().begin(); property != this->dto.getPropertiesList().end();
                 ++
                 property) {
                if ((*property)->name != "id" && (*property)->get(this->dto.get()) != nullptr) {
                    params.emplace((*property)->name, (*property)->get(this->dto.get()));
                }
            }
            int cnt = 0;
            for (const auto &item: params | std::ranges::views::keys) {
                cnt++;
                sql_params << *item;
                sql_values << ":" << *item;
                if (cnt != params.size()) {
                    sql_params << ", ";
                    sql_values << ", ";
                }
            }
            sql << "INSERT INTO " << TABLE_NAME << " (" << sql_params.str() << ") VALUES (" << sql_values.str()
                    << ") returning id;";

            auto res = this->db->executeQuery(sql.str(), params);
            auto vec = res->template fetch<Vector<Object<DTO_TYPE> > >();
            if (!res->isSuccess() || vec->empty()) {
                std::ostringstream warning;
                warning << "Failed to write data on table: `" << TABLE_NAME << "`. Reason: " << *res->getErrorMessage();
                throw HttpError(Status::CODE_500, warning.str());
            }
            this->dto->id = vec->front()->id;
        }

        static std::shared_ptr<ServiceBean> get(const std::shared_ptr<orm::QueryResult> &res) {
            using namespace web::protocol::http;
            if (!res->isSuccess()) {
                std::ostringstream warning;
                warning << "Failed to read data from table: `" << TABLE_NAME << "`. Reason: " << *res->
                        getErrorMessage();
                throw HttpError(Status::CODE_500, warning.str());
            }
            if (auto vec = res->fetch<Vector<Object<DTO_TYPE> > >(); !vec->empty()) {
                return createShared(vec->front());
            }
            return nullptr;
        }

        template<typename T=ServiceBean>
        static std::shared_ptr<T> getById(const ID_TYPE &id) {
            using namespace web::protocol::http;
            OATPP_COMPONENT(std::shared_ptr<PgDb>, db);
            boost::format fmt("SELECT %2% FROM %1% WHERE id = :id");
            fmt % TABLE_NAME;
            fmt % getFieldStr();
            auto res = db->executeQuery(fmt.str(), {{"id", id}});
            return std::static_pointer_cast<T>(get(res));
        }

        static Vector<Object<DTO_TYPE> > toDtoList(auto list) {
            if (std::holds_alternative<Vector<Object<DTO_TYPE> > >(list)) {
                return std::get<Vector<Object<DTO_TYPE> > >(list);
            }
            return nullptr;
        }

        static std::vector<std::shared_ptr<ServiceBean> > toWrappedList(auto list) {
            if (std::holds_alternative<std::vector<std::shared_ptr<ServiceBean> > >(list)) {
                return std::get<std::vector<std::shared_ptr<ServiceBean> > >(list);
            }
            return {};
        }

        template<typename T>
        static std::vector<std::shared_ptr<T> > castWList(auto list) {
            std::vector<std::shared_ptr<T> > res = {};
            res.reserve(list.size());
            for (const auto item: list) {
                res.emplace_back(std::static_pointer_cast<T>(item));
            }
            return res;
        }

        static std::variant<std::vector<std::shared_ptr<ServiceBean> >, Vector<Object<DTO_TYPE> > >
        getList(const std::shared_ptr<orm::QueryResult> &res, const Boolean &wrapped = false) {
            using namespace web::protocol::http;
            if (!res->isSuccess()) {
                std::ostringstream warning;
                warning << "Failed to list data from table: `" << TABLE_NAME << "`. Reason: " << *res->
                        getErrorMessage();
                throw HttpError(Status::CODE_500, warning.str());
            }
            auto vec = res->fetch<Vector<Object<DTO_TYPE> > >();
            if (!wrapped) {
                return vec;
            }
            std::vector<std::shared_ptr<ServiceBean> > bean_vec;
            bean_vec.reserve(vec->size());
            for (const auto &dto: *vec) {
                bean_vec.push_back(createShared(dto));
            }
            return bean_vec;
        }

        static std::variant<std::vector<std::shared_ptr<ServiceBean> >, Vector<Object<DTO_TYPE> > >
        getAll(const Boolean &wrapped = false) {
            OATPP_COMPONENT(std::shared_ptr<PgDb>, db);
            std::ostringstream sql;
            sql << "SELECT " << getFieldStr() << "  FROM " << TABLE_NAME;
            return getList(db->executeQuery(sql.str(), {}));
        }

        static ID_TYPE count() {
            using namespace web::protocol::http;
            OATPP_COMPONENT(std::shared_ptr<PgDb>, db);
            std::ostringstream sql;
            sql << "SELECT count(id) FROM " << TABLE_NAME << ";";
            return count(db->executeQuery(sql.str(), {}));
        }

        static ID_TYPE count(const std::shared_ptr<oatpp::orm::QueryResult> &res) {
            using namespace web::protocol::http;
            if (!res->isSuccess()) {
                std::ostringstream warning;
                warning << "Failed to count size of this table: " << TABLE_NAME << "Reason: " << *res->
                        getErrorMessage();
                throw HttpError(Status::CODE_403, warning.str());
            }
            auto vec = res->fetch<Vector<Object<CountDto<ID_TYPE> > > >();
            return vec->front()->count;
        }

        template<int max_page_size = 100>
        static std::variant<std::vector<std::shared_ptr<ServiceBean> >, Vector<Object<DTO_TYPE> > >
        page(const Int32 &page_size, const Int32 &page_num, const Boolean &wrapped = false, const String &order = "id",
             const Boolean &desc = false) {
            checkPage(page_size, page_num, max_page_size);
            OATPP_COMPONENT(std::shared_ptr<PgDb>, db);
            boost::format fmt("SELECT %4% FROM %1% ORDER BY %2% LIMIT :limit OFFSET :offset %3%");
            fmt % TABLE_NAME;
            fmt % *order;
            fmt % (desc ? "DESC" : "");
            fmt % getFieldStr();
            const auto res = db->executeQuery(fmt.str(), {
                                                  {"limit", page_size},
                                                  {"offset", Int32((page_num - 1) * page_size)}
                                              });
            return getList(res, wrapped);
        }

        template<int max_page_size = 100>
        static auto
        page(const Object<PagedRequestDto> &dto, const Boolean &wrapped = false, const String &order = "id",
             const Boolean &desc = false) {
            return page<max_page_size>(dto->page_size, dto->page_num, wrapped, order, desc);
        }

        template<int max_page_size = 100>
        static auto
        page(const std::shared_ptr<oatpp::orm::QueryResult> &res, const oatpp::Object<PagedRequestDto> &request,
             const Boolean &wrapped = false) {
            using namespace web::protocol::http;
            checkPage(request->page_size, request->page_num, max_page_size);
            return getList(res, wrapped);
        }

        void overwrite(const Object<DTO_TYPE> &_dto, const std::unordered_set<std::string> &allow_fields = {}) {
            using namespace web::protocol::http;
            std::unordered_map<String, Void> params;
            std::ostringstream sql;
            sql << "UPDATE " << TABLE_NAME << " SET ";
            auto allow_all = false;
            if (allow_fields.empty()) allow_all = true;
            for (auto &pair: Object<DTO_TYPE>::getPropertiesMap()) {
                if (pair.first == "id") continue;
                auto later = pair.second->get(_dto.get());
                auto previous = pair.second->get(this->dto.get());
                if (later != nullptr && later != previous) {
                    if (!allow_all and !allow_fields.contains(pair.first)) {
                        std::ostringstream warning;
                        warning << "Not allowed to overwrite this field: " << pair.first;
                        throw HttpError(Status::CODE_403, warning.str());
                    }
                    params.emplace(pair.first, later);
                    this->dto[pair.first] = later;
                }
            }
            int cnt = 0;
            for (auto front = params.begin(); front != params.end(); ++front) {
                sql << *front->first << "=:" << *front->first;
                cnt++;
                if (cnt != params.size()) {
                    sql << ", ";
                } else {
                    sql << " WHERE id = :id";
                }
            }
            params.emplace("id", this->dto->id);
            this->db->executeQuery(sql.str(), params);
        }

        void del() {
            using namespace web::protocol::http;
            std::ostringstream sql;
            sql << "DELETE FROM " << TABLE_NAME << " WHERE id = :id";
            if (auto res = this->db->executeQuery(sql.str(), {{"id", this->dto->id}}); !res->isSuccess()) {
                std::ostringstream warning;
                warning << "Failed to delete data from table: `" << TABLE_NAME << "`. Reason: " << *res->
                        getErrorMessage();
                throw HttpError(Status::CODE_500, warning.str());
            }
        }

        static auto getByField(const String &field_name, const Void &field_value, const Boolean &wrapped = false) {
            OATPP_COMPONENT(std::shared_ptr<PgDb>, db);
            using namespace web::protocol::http;
            std::unordered_map<String, Void> params;
            std::ostringstream sql;
            sql << "SELECT " << getFieldStr() << " FROM " << TABLE_NAME << " WHERE " << *field_name << "=:" << *
                    field_name;
            return getList(db->executeQuery(sql.str(), {{field_name, field_value}}), wrapped);
        }
    };
}
#endif //ENDER_LABEL_SERVICEBEAN_HPP
