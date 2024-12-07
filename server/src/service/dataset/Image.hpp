//
// Created by ender on 12/7/24.
//

#ifndef IMAGE_HPP
#define IMAGE_HPP
#include <opencv2/imgcodecs.hpp>

namespace ender_label::service::dataset {
    constexpr char table_name_image[] = "ender_label_image";
    namespace fs = std::filesystem;

    class Image final : public ServiceBean<table_name_image, data::ImageDto, Int64> {
    public:
        auto readFromFile(const std::filesystem::path &img_file) {
            if (not exists(img_file)) {
                throw std::runtime_error("Requested path does not exist.");
            }
            const auto img = cv::imread(img_file.c_str());
            const auto dto = data::ImageDto::createShared();
            this->getDto()->size = file_size(img_file);
            this->getDto()->relative_path = img_file.string();
            this->getDto()->height = img.rows;
            this->getDto()->width = img.cols;
            this->getDto()->md5_hash_32 = util::Util::computeFileMd5(img_file);
        }

        static auto createFromFile(const std::filesystem::path &img_file) {
            auto obj = std::make_shared<Image>();
            obj->readFromFile(img_file);
            obj->write();
            return obj;
        }

        void del() override {
            const auto path = fs::path(this->getDto()->relative_path);
            fs::remove(path);
            ServiceBean::del();
        }
    };
}


#endif //IMAGE_HPP
