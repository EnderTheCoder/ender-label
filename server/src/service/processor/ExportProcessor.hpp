//
// Created by ender on 12/14/24.
//

#ifndef EXPORTPROCESSOR_HPP
#define EXPORTPROCESSOR_HPP
#include <future>
#include <db/PgDb.hpp>
#include <util/TimeUtil.hpp>

#include "service/dataset/ExportLog.hpp"

namespace ender_label::service::processor {
    class ExportProcessor {
        std::unordered_set<int32_t> working_datasets = {};
        const OATPP_COMPONENT(std::shared_ptr<db::PgDb>, db);

        auto initState() {
            const auto res = db->executeQuery(
                "UPDATE ender_label_export_log SET state = 'TIMEOUT', end_time = int8(EXTRACT(EPOCH FROM NOW())) WHERE state = 'WORKING'",
                {});
        }

    public:
        ExportProcessor() {
            initState();
        }

        static auto createShared() {
            return std::make_shared<ExportProcessor>();
        }

        auto getIsDatasetExporting(const auto &id) {
            return this->working_datasets.contains(id);
        }

        auto exportWithProxy(std::future<void> export_future, const auto &dataset_id, const auto &user_id,
                             const auto &archive_path) {
            const auto n_dto = data::ExportLogDto::createShared();
            n_dto->dataset_id = dataset_id;
            n_dto->user_id = user_id;
            n_dto->begin_time = util::TimeUtil::getCurrentTimestampInLong();
            n_dto->state = data::ExportLogType::WORKING;
            n_dto->archive_path = archive_path;
            const auto export_log = dataset::ExportLog::createShared(n_dto);
            export_log->write();
            this->working_datasets.emplace(*export_log->getId());
            std::thread t([&export_future, export_log, this]() {
                try {
                    export_future.get();
                    const auto o_dto = data::ExportLogDto::createShared();
                    o_dto->end_time = util::TimeUtil::getCurrentTimestampInLong();
                    o_dto->state = data::ExportLogType::COMPLETED;
                    o_dto->info = "OK";
                    export_log->overwrite(o_dto);
                } catch (std::exception &e) {
                    const auto o_dto = data::ExportLogDto::createShared();
                    o_dto->end_time = util::TimeUtil::getCurrentTimestampInLong();
                    o_dto->state = data::ExportLogType::EXCEPTION;
                    o_dto->info = e.what();
                    export_log->overwrite(o_dto);
                }
                this->working_datasets.erase(*export_log->getId());
            });
            t.detach();
        }
    };
}


#endif //EXPORTPROCESSOR_HPP
