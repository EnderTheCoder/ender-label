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
            return root / "thumbnail";
        }

        static void initThumbnailDir() {
            if (const auto thumbnail = getThumbnailDir(); not exists(thumbnail)) {
                create_directory(thumbnail);
            }
        }

        static auto imgThumbnailPath(const auto& img_hash) {
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
                    for (const auto &img: dataset::Image::toWrappedList(dataset::Image::getAll())) {
                        const auto img_u = std::static_pointer_cast<dataset::Image>(img);
                        if (const auto img_thumbnail_path = imgThumbnailPath(img_u->getDto()->md5_hash_32); not
                            exists(img_thumbnail_path)) {
                            OATPP_LOGI("Thumbnail", "Thumbnail for img[%lld] does not exist, start generating.",
                                       *img_u->getId())
                            auto mat = img_u->genThumbnail();
                            imwrite(thumbnail / *(img_u->getDto()->md5_hash_32 + ".png"), mat);
                        }
                    }
                    std::this_thread::sleep_for(100ms);
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
