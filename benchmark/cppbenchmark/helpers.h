//
//  helpers.h
//  libdatachannel
//
//  Created by Savolainen, Petri T E on 02/03/2021.
//

#ifndef helpers_h
#define helpers_h

#include <random>
#include <string>
#include <chrono>
#include <ctime>


// strftime format
#define LOGGER_PRETTY_TIME_FORMAT "%Y-%m-%d %H:%M:%S"

// printf format
#define LOGGER_PRETTY_MS_FORMAT ".%03d"

using namespace std;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::system_clock;

namespace g2{
typedef std::chrono::time_point<std::chrono::system_clock>  system_time_point;
 
inline tm localtime(const std::time_t& time)
{
  std::tm tm_snapshot;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
  localtime_s(&tm_snapshot, &time);
#else
  localtime_r(&time, &tm_snapshot); // POSIX
#endif
  return tm_snapshot;
}
 
// To simplify things the return value is just a string. I.e. by design!
inline std::string put_time(const std::tm& date_time, const char* c_time_format)
{
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
  std::ostringstream oss;
 
  // BOGUS hack done for VS2012: C++11 non-conformant since it SHOULD take a "const struct tm*  "
  // ref. C++11 standard: ISO/IEC 14882:2011, § 27.7.1,
  oss << std::put_time(const_cast<std::tm*>(date_time), c_time_format);
  return oss.str();
 
#else    // LINUX
  const size_t size = 1024;
  char buffer[size];
  auto success = std::strftime(buffer, size, c_time_format, &date_time);
 
  if (0 == success)
    return c_time_format;
 
  return buffer;
#endif
}
 
// extracting std::time_t from std:chrono for "now"
inline std::time_t systemtime_now()
{
  system_time_point system_now = std::chrono::system_clock::now();
  return std::chrono::system_clock::to_time_t(system_now);
}
 
} // g2-namespace

namespace Helpers
{
// Helper function to generate a random String
inline string randomString(size_t length)
    {
    static const string characters(
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    string id(length, '0');
    default_random_engine rng(random_device{}());
    uniform_int_distribution<int> dist(0, int(characters.size() - 1));
    generate(id.begin(), id.end(), [&]() { return characters.at(dist(rng)); });
    return id;
    }

inline int formatRate(int bytes)
    {
    return bytes / 1024;
    }


inline string now()
	{
    return g2::put_time(g2::localtime(g2::systemtime_now()), LOGGER_PRETTY_TIME_FORMAT) + " ";
	}

inline uint64_t millisecondsNow()
    {
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }

}

#endif /* helpers_h */
