//
// Created by ender on 12/7/24.
//

#ifndef IMAGE_HPP
#define IMAGE_HPP
#include <opencv2/opencv.hpp>

namespace ender_label::service::dataset {
    constexpr char table_name_image[] = "ender_label_image";
    namespace fs = std::filesystem;

    class Image final : public ServiceBean<table_name_image, data::ImageDto, Int64> {
        auto readCvImgFromDisk() {
            const auto img = cv::imread(this->getDto()->relative_path);
            if (img.empty()) {
                throw std::runtime_error("Could not read img from given path: " + this->getDto()->relative_path);
            }
            return img;
        }

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


        auto genThumbnail() {
            const auto path = fs::path{this->getDto()->relative_path};
            const auto img = readCvImgFromDisk();
            int height = img.rows;
            int width = img.cols;
            float scale = std::min(100.0f / width, 100.0f / height);
            int newWidth = static_cast<int>(width * scale);
            int newHeight = static_cast<int>(height * scale);
            cv::Mat resizedImg;
            resize(img, resizedImg, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_AREA);
            int top = (100 - newHeight) / 2;
            int bottom = (100 - newHeight) - top;
            int left = (100 - newWidth) / 2;
            int right = (100 - newWidth) - left;
            cv::Scalar color(0, 0, 0, 0);
            copyMakeBorder(resizedImg, resizedImg, top, bottom, left, right, cv::BORDER_CONSTANT, color);
            cvtColor(resizedImg, resizedImg, cv::COLOR_BGR2BGRA);
            return resizedImg;
        }

        void del() override {
            const auto path = fs::path(this->getDto()->relative_path);
            fs::remove(path);
            ServiceBean::del();
        }
    };
}


#endif //IMAGE_HPP
