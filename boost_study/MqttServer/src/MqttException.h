
#include <boost/stacktrace.hpp>
#include <exception>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

/* struct Exception{
    Exception(int errorCode,std::string_view errorMsg) : errorCode_(errorCode),errorMsg_(errorMsg){}
    int errorCode_;
    std::string errorMsg_;
};

class ExceptionEnum {
public:
    Exception DISCON_COMMAND_ERR =  Exception(1000,"连接命令错误");
    static ExceptionEnum& getInstance() {
        static ExceptionEnum instance;
        return instance;
    }
    ExceptionEnum(const ExceptionEnum& ) = delete;
    ExceptionEnum(ExceptionEnum&& ) = delete;
    ExceptionEnum& operator=(const ExceptionEnum&) = delete;
    ExceptionEnum& operator=(ExceptionEnum&& ) = delete;
    ~ExceptionEnum() = default;
private:
    ExceptionEnum();
}; */

class MqttException : public std::exception {
public:
    explicit MqttException(int errorCode, std::string_view errorMsg)
        : std::exception(),
          errorCode_(errorCode),
          errorMsg_(errorMsg),
          stackrace_(boost::stacktrace::stacktrace()) {}

    const char* what() const noexcept override { return errorMsg_.c_str(); }

    MqttException(MqttException&& other)
        : errorCode_(other.errorCode_),
          errorMsg_(std::move(other.errorMsg_)),
          stackrace_(std::move(other.stackrace_)) {}

    int code() const { return errorCode_; }

    std::string getStackTrace() const {
        std::ostringstream oss;
        oss << stackrace_;
        return oss.str();
    }

private:
    int errorCode_;
    std::string errorMsg_;
    boost::stacktrace::stacktrace stackrace_;
};