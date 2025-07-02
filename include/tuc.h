#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include "tuc/small_tools.h"
#include "tuc/user.h"
#include "tuc/print.h"
#include "crow_all.h"

#define TUC_VERSION "0.1 Alpha"
#define TUC_COMMIT "?" // 

class shell {
    private:
        crow::SimpleApp crow_app;
        std::map<std::string, user> users;
        void routes() {
            CROW_ROUTE(crow_app, "/users")
            ([this](){
                nlohmann::json json_data;
                nlohmann::json user_json;
                for (const auto& pair : users) {
                    const std::string& username = pair.first;
                    user user_data = pair.second;
                    user_json["name"] = user_data.get_name();
                    user_json["hashed_password"] = user_data.get_hashed_password();
                    user_json["permissions"] = static_cast<int>(user_data.get_permissions());
                    json_data[username] = user_json;
                }
                return json_data;
            });
        }
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