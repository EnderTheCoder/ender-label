//
// Created by ender on 12/14/24.
//

#ifndef EXPORTPROCESSOR_HPP
#define EXPORTPROCESSOR_HPP
#include <future>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/detail/throw_error.hpp>
#include <boost/filesystem/operations.hpp>
#include <db/PgDb.hpp>
#include <util/TimeUtil.hpp>

#include "service/dataset/ExportLog.hpp"

namespace ender_label::service::processor {
    class ExportProcessor {
        std::mutex dataset_lck;
        std::unordered_set<int32_t> working_datasets = {};
        boost::asio::thread_pool pool = {16};
        const OATPP_COMPONENT(std::shared_ptr<db::PgDb>, db);

        auto initState() const {
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

        static auto createZip(const std::filesystem::path &src, const std::filesystem::path &dst,
                              const auto &del_source = true) -> void {
            boost::format fmt("cd %1% && zip -r -q %2% %3%");
            fmt % src.parent_path().c_str() % dst.filename().c_str() % src.filename().c_str();
            system(fmt.str().c_str());
            if (del_source) {
                remove_all(src);
            }
        }

        auto getIsDatasetExporting(const auto &id) {
            std::lock_guard guard(this->dataset_lck);
            return this->working_datasets.contains(id);
        }

        auto exportWithProxy(std::future<void> export_future, const auto &dataset_id, const auto &user_id,
                             const auto &export_path) {
            const auto n_dto = data::ExportLogDto::createShared();
            n_dto->dataset_id = dataset_id;
            n_dto->user_id = user_id;
            n_dto->begin_time = util::TimeUtil::getCurrentTimestampInLong();
            n_dto->state = data::ExportLogState::WORKING;
            const auto archive_path = std::filesystem::path(export_path.string() + ".zip");
            n_dto->archive_path = archive_path.string();
            const auto export_log = dataset::ExportLog::createShared(n_dto);
            export_log->write(); {
                std::lock_guard guard(this->dataset_lck);
                this->working_datasets.emplace(*export_log->getId());
            }
            boost::asio::post(
                this->pool, [export_future = std::move(export_future), export_path,archive_path, export_log, this]() {
                    try {
                        export_future.wait();
                        ExportProcessor::createZip(export_path, archive_path, true);
                        const auto o_dto = data::ExportLogDto::createShared();
                        o_dto->end_time = util::TimeUtil::getCurrentTimestampInLong();
                        o_dto->state = data::ExportLogState::COMPLETED;
                        o_dto->info = "OK";
                        export_log->overwrite(o_dto);
                    } catch (std::exception &e) {
                        const auto o_dto = data::ExportLogDto::createShared();
                        o_dto->end_time = util::TimeUtil::getCurrentTimestampInLong();
                        o_dto->state = data::ExportLogState::EXCEPTION;
                        o_dto->info = e.what();
                        export_log->overwrite(o_dto);
                    } {
                        std::lock_guard guard(this->dataset_lck);
                        this->working_datasets.erase(*export_log->getId());
                    }
                });
        }
    };
}


#endif //EXPORTPROCESSOR_HPP
