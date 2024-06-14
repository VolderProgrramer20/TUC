#include <string>
#include "crow_all.h"

class tuc_logger : public crow::ILogHandler {
    private:
        struct message_data {
            std::string message;
            crow::LogLevel level;
        };
        std::vector<message_data> messages;
    public:
        tuc_logger() {}
        void log(std::string message, crow::LogLevel level) {
            std::cerr << message << std::endl;
            messages.push_back({message, level});
        }
        std::string get_log_message() {
            if (messages.size() < 2) {
                return messages[messages.size()].message;
            }
            return messages[messages.size() - 1].message;
        }
        crow::LogLevel get_loglevel_message() {
            if (messages.size() < 2) {
                return messages[messages.size()].level;
            }
            return messages[messages.size() - 1].level;
        }
};