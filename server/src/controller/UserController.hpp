//
// Created by ender on 11/26/24.
//

#ifndef USERCONTROLLER_HPP
#define USERCONTROLLER_HPP

#include "service/user/Permission.hpp"
#include "service/user/User.hpp"
#include "dto/request/LoginRequestDto.hpp"
#include "dto/response/SimpleDataResponseDto.hpp"
#include "util/ControllerUtil.hpp"
#include "util/AuthUtil.hpp"
#include "dto/request/RegisterRequestDto.hpp"
#include "dto/response/ArrayResponseDto.hpp"
#include OATPP_CODEGEN_BEGIN(ApiController)

namespace ender_label::controller {
    using namespace service;

    class UserController final : public oatpp::web::server::api::ApiController {
    private:
        explicit UserController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
            : oatpp::web::server::api::ApiController(objectMapper) {
            this->setErrorHandler(std::make_shared<component::ErrorHandler>(objectMapper));
        }

    public:
        static std::shared_ptr<UserController>
        createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) {
            return std::shared_ptr<UserController>(new UserController(objectMapper));
        }

    public:
        ENDPOINT("POST", "/user/login", login, BODY_DTO(Object<request::LoginRequestDto>, req)) {
            REQUEST_ALL_PARAM_CHECK(req)
            const auto resp = SimpleDataResponseDto<Object<data::UserDto> >::createShared();
            const auto users = user::User::toWrappedList(user::User::getByField("username", req->username, true));
            if (users.empty() or users.front()->getDto()->password != req->password) {
                resp->code = -100;
                resp->message = "Wrong username or password";
                return createDtoResponse(Status::CODE_200, resp);
            }
            const auto user = std::static_pointer_cast<user::User>(users.front());
            user->refreshSession();
            resp->data = user->getDto();
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(login) {
            info->name = "登录";
            info->description = "使用帐号密码登陆，在返回数据的data->session中获取后续请求其他接口用于认证的token，保存到前端缓存。";
            info->addConsumes<Object<request::LoginRequestDto> >("application/json");
            info->addResponse<Object<SimpleDataResponseDto<Object<data::UserDto> > > >(
                Status::CODE_200, "applications/json");
        }

        ENDPOINT("POST", "/user/register", _register, BODY_DTO(Object<request::RegisterRequestDto>, req)) {
            const auto resp = SimpleDataResponseDto<Object<data::UserDto> >::createShared();
            REQUEST_ALL_PARAM_CHECK(req)
            OATPP_ASSERT_HTTP(req->password->size() >= 8, Status::CODE_411, "Password requires at least 8 characters.")
            const auto user_dto = data::UserDto::createShared();
            const auto perm_create_database = user::Permission::fromKey("DATASET_CREATE");
            const auto perm_list_database = user::Permission::fromKey("DATASET_LIST");
            OATPP_ASSERT_HTTP(perm_create_database != nullptr, Status::CODE_500, "Missing system built-in perm.")
            OATPP_ASSERT_HTTP(perm_list_database != nullptr, Status::CODE_500, "Missing system built-in perm.")
            if (not user::User::toDtoList(user::User::getByField("username", req->username))->empty()) {
                resp->code = -100;
                resp->message = "Username already in use.";
                return createDtoResponse(Status::CODE_200, resp);
            }
            user_dto->permission_ids = {perm_list_database->getId(), perm_create_database->getId()};
            user_dto->email = req->email;
            user_dto->password = req->password;
            user_dto->username = req->username;
            user_dto->token = nullptr;
            const auto user = user::User::createShared<user::User>(user_dto);
            user->write();
            resp->data = user->getDto();
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(_register) {
            info->name = "注册";
            info->description = "注册后会返回用户数据。密码要求至少8字符。username，password，email三个字段不能为空.";
            info->addConsumes<Object<request::RegisterRequestDto> >("application/json");
            info->addResponse<Object<SimpleDataResponseDto<Object<data::UserDto> > > >(
                Status::CODE_200, "applications/json");
        }

        ENDPOINT("GET", "/user/permission/all", getAllPermissions, AUTH_HEADER) {
            AUTH
            const auto resp = ArrayResponseDto<Object<data::PermissionDto> >::createShared();
            resp->data = user::Permission::toDtoList(user::Permission::getAll());
            resp->size = resp->data->size();
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(getAllPermissions) {
            info->name = "查询所有权限";
            info->description = "返回一个映射表，主要表现权限id和权限的对应关系，根据该表修改用户的权限。";
            info->addResponse<Object<ArrayResponseDto<Object<data::PermissionDto> > > >(
                Status::CODE_200, "applications/json");
        }

        ENDPOINT("POST", "/user/{uid}/permission/ch", chPermission, AUTH_HEADER, PATH(Int32, uid),
                 BODY_DTO(UnorderedSet<Int32>, perms)) {
            AUTH
            if (!USER->hasPerm("ROOT")) {
                ERROR(Status::CODE_403, "Permission denied.")
            }
            for (const auto &perm_id: *perms) {
                auto perm = user::Permission::getById(perm_id);
                OATPP_ASSERT_HTTP(
                    perm != nullptr,
                    Status::CODE_404,
                    "Request perm id does not exist: " + std::to_string(perm_id)
                )
            }
            const auto dto = data::UserDto::createShared();
            auto user = user::User::getById<user::User>(uid);
            OATPP_ASSERT_HTTP(user != nullptr, Status::CODE_404, "User not found.")
            user->overwrite(dto);
            return createDtoResponse(Status::CODE_200, BaseResponseDto::createShared());
        }

        ENDPOINT_INFO(chPermission) {
            info->name = "修改用户权限";
            info->description = "需要管理员权限。提交新的权限表覆盖原有权限表。提交一个列表包含权限id，例如[1,3,4]则代表赋予用户id为1,3,4的权限节点。";
            info->addConsumes<UnorderedSet<Int32> >("application/json", "权限ids");
            info->addResponse<Object<BaseResponseDto> >(Status::CODE_200, "applications/json");
        }

        ENDPOINT("POST", "/user/add", addUser, AUTH_HEADER, BODY_DTO(Object<data::UserDto>, user_dto)) {
            AUTH
            REQUEST_PARAM_CHECK(user_dto->email)
            REQUEST_PARAM_CHECK(user_dto->password)
            REQUEST_PARAM_CHECK(user_dto->permission_ids)
            REQUEST_PARAM_CHECK(user_dto->username)
            if (!USER->hasPerm("ROOT")) {
                ERROR(Status::CODE_403, "Permission denied.")
            }
            const auto user = user::User::createShared(user_dto);
            user->write();
            return createDtoResponse(Status::CODE_200, BaseResponseDto::createShared());
        }

        ENDPOINT_INFO(addUser) {
            info->name = "创建用户";
            info->description = "需要管理员权限。使用管理员权限创建一个新的用户，注意除了id字段都需要不为空，id字段省略。";
            info->addConsumes<Object<data::UserDto> >("application/json");
            info->addResponse<Object<BaseResponseDto> >(Status::CODE_200, "application/json");
            // info->addSecurityRequirement("ROOT");
        }

        ENDPOINT("GET", "/user/{uid}/rm", rmUser, AUTH_HEADER, PATH(Int32, uid)) {
            AUTH
            if (!USER->hasPerm("ROOT")) {
                ERROR(Status::CODE_403, "Permission denied.")
            }
            auto user = user::User::getById<user::User>(uid);
            OATPP_ASSERT_HTTP(user != nullptr, Status::CODE_404, "User not found.")
            user->del();
            return createDtoResponse(Status::CODE_200, BaseResponseDto::createShared());
        }

        ENDPOINT_INFO(rmUser) {
            info->name = "删除用户";
            info->description = "需要管理员权限。提交被删除用户的ID，以删除该用户。";
            info->addResponse<Object<BaseResponseDto> >(Status::CODE_200, "application/json");
            // info->addSecurityRequirement("ROOT");
        }

        ENDPOINT("GET", "/user/{uid}/info", getUser, AUTH_HEADER, PATH(Int32, uid)) {
            AUTH
            const auto resp = SimpleDataResponseDto<Object<data::UserDto> >::createShared();
            auto user = user::User::getById<user::User>(uid);
            OATPP_ASSERT_HTTP(user != nullptr, Status::CODE_404, "User not found.")
            resp->data = user->getDto();
            resp->data->password = nullptr;
            resp->data->token = nullptr;
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(getUser) {
            info->name = "查看用户详情";
            info->description = "登陆后可以查看用户详情。某些含有关键信息的字段被屏蔽为null。";
            info->addResponse<Object<SimpleDataResponseDto<Object<data::UserDto> > > >(
                Status::CODE_200, "application/json");
        }

        ENDPOINT("GET", "/user/search", searchUser, AUTH_HEADER, QUERY(String, keyword)) {
            AUTH
            const OATPP_COMPONENT(std::shared_ptr<PgDb>, db);
            const auto resp = ArrayResponseDto<Object<data::UserDto> >::createShared();
            std::stringstream ss;
            ss << "SELECT * FROM " << user::User::getTableName() << " WHERE username like concat(:username, '%');";
            const auto res = db->executeQuery(ss.str(), {
                                                  {"username", keyword}
                                              });
            resp->data = user::User::toDtoList(user::User::getList(res));
            resp->size = resp->data->size();
            std::for_each(resp->data->begin(), resp->data->end(), [](auto &x) {
                x->password = nullptr;
                x->token = nullptr;
            });
            return createDtoResponse(Status::CODE_200, resp);
        }

        ENDPOINT_INFO(searchUser) {
            info->name = "搜索用户";
            info->description = "根据用户名搜索用户。";
            info->addResponse<Object<ArrayResponseDto<Object<data::UserDto> > > >(
                Status::CODE_200, "application/json");
        }
    };
}

#include OATPP_CODEGEN_END(ApiController)

#endif //USERCONTROLLER_HPP
