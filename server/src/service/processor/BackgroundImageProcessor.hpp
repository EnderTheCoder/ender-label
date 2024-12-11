//
// Created by ender on 12/7/24.
//

#ifndef BACKGROUNDIMAGEPROCESSOR_HPP
#define BACKGROUNDIMAGEPROCESSOR_HPP
#include <service/dataset/Image.hpp>

namespace ender_label::service::processor {
    class BackgroundImageProcessor {
        std::atomic<bool> sig_terminate = false;

    public:
        static auto getThumbnailDir() {
            OATPP_COMPONENT(oatpp::Object<data::ConfigDto>, config);
            std::filesystem::path root{config->storage};
            return root / "thumbnails";
        }

        static void initThumbnailDir() {
            if (const auto thumbnail = getThumbnailDir(); not exists(thumbnail)) {
                OATPP_LOGI("THUMBNAIL", "Thumbnail root dir does not exist, creating.")
                create_directory(thumbnail);
            }
        }

        static auto imgThumbnailPath(const auto &img_hash) {
            return getThumbnailDir() / *(img_hash + ".png");
        }

        static auto createShared() {
            return std::make_shared<BackgroundImageProcessor>();
        }

        void start() const {
            std::thread t([this] {
                using namespace std::chrono_literals;
                while (true) {
                    if (this->sig_terminate) return;
                    auto thumbnail = getThumbnailDir();
                    // gen thumbnail
                    auto success_gen = 0;
                    for (const auto &img: dataset::Image::toWrappedList(dataset::Image::getAll(true))) {
                        const auto img_u = std::static_pointer_cast<dataset::Image>(img);
                        if (const auto img_thumbnail_path = imgThumbnailPath(img_u->getDto()->md5_hash_32); not
                            exists(img_thumbnail_path)) {
                            OATPP_LOGI("THUMBNAIL", "Thumbnail for img[%lld] does not exist, start generating.",
                                       *img_u->getId())
                            auto mat = img_u->genThumbnail();
                            imwrite(thumbnail / *(img_u->getDto()->md5_hash_32 + ".png"), mat);
                            success_gen++;
                        }
                    }
                    if (success_gen != 0) {
                        OATPP_LOGI("THUMBNAIL", "Thumbnail gen complete: %d succeed.", success_gen)
                    }
                    std::this_thread::sleep_for(1000ms);
                }
            });
            t.detach();
        }

        void stop() {
            this->sig_terminate = true;
        }
    };
}


#endif //BACKGROUNDIMAGEPROCESSOR_HPP
