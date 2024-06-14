#include <string>
#include <map>
#include "tuc/small_tools.h"
#include "tuc/logs.h"
#include "tuc/user.h"
#include "crow_all.h"

#define TUC_VERSION "0.1 Alpha"
#define TUC_COMMIT "?" // 

class shell {
    private:
        crow::SimpleApp crow_app;
        tuc_logger logger;
        std::map<std::string, user> users;
    public:
        void logLevel(int level) {
            if (level < 0 || level > 4) return;
            if (level == 0) crow_app.loglevel(crow::LogLevel::Debug);
            if (level == 1) crow_app.loglevel(crow::LogLevel::Info);
            if (level == 2) crow_app.loglevel(crow::LogLevel::Warning);
            if (level == 3) crow_app.loglevel(crow::LogLevel::Error);
            if (level == 4) crow_app.loglevel(crow::LogLevel::Critical);
        }
        void start() {
            crow::logger::setHandler(&logger);
            crow_app.port(18080).multithreaded().run();
        }
        void start(int port) {
            crow::logger::setHandler(&logger);
            crow_app.port(port).multithreaded().run();
        }
};