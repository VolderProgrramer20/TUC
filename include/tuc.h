#include <string>
#include <map>
#include "tuc/small_tools.h"
#include "tuc/user.h"
#include "crow_all.h"

#define TUC_VERSION "0.1 Alpha"
#define TUC_COMMIT "?" // 

class shell {
    private:
        crow::SimpleApp crow_app;
        std::map<std::string, user> users;
        /*
        void basic() {
            CROW_GET(crow_app, "/core/core.js")([]() {
                mustache::context ctx;
                return mustache::load_text("core.js");
            });
        }
        */
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
            crow_app.port(18080).multithreaded().run();
        }
        void start(int port) {
            crow_app.port(port).multithreaded().run();
        }
};