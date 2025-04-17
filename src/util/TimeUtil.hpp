//
// Created by ender on 22-9-21.
//

#ifndef ENDER_LABEL_TIMEUTIL_HPP
#define ENDER_LABEL_TIMEUTIL_HPP

#include <string>
#include <memory>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/time_clock.hpp>
#include <iomanip>
#include "oatpp/core/Types.hpp"
namespace ender_label::util{
class TimeUtil {
public:
    static long getCurrentTimestampInLong() {
        boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
        boost::posix_time::time_duration timestamp = boost::posix_time::second_clock::universal_time() - epoch;
        return timestamp.total_seconds();
    }


    static auto getCurrentTimeInStr()
    {
        time_t now = time(nullptr);
        tm *ltm = localtime(&now);
        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0') << ltm->tm_year + 1900;
        ss << std::setw(2) << std::setfill('0') << ltm->tm_mon + 1;
        ss << std::setw(2) << std::setfill('0') << ltm->tm_mday;
        ss << std::setw(2) << std::setfill('0') << ltm->tm_hour;
        ss << std::setw(2) << std::setfill('0') << ltm->tm_min;
        ss << std::setw(2) << std::setfill('0') << ltm->tm_sec;
        return ss.str();
    }

    static auto getCurrentTimeRFC3339()
    {
        auto now = std::chrono::system_clock::now();
        auto itt = std::chrono::system_clock::to_time_t(now);
        std::ostringstream ss;
        ss << std::put_time(gmtime(&itt), "%FT%TZ");
        return ss.str();
    }

    static auto getCurrentTimeStd() {
        return time(nullptr);
    }

    static auto generateRFC3339(time_t time) {
        std::ostringstream ss;
        ss << std::put_time(gmtime(&time), "%FT%TZ");
        return ss.str();
    }

};
}
#endif //ENDER_LABEL_TIMEUTIL_HPP
