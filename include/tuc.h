#include <string>
#include "tuc/small_tools.h"
#include "tuc/logs.h"
#include "tuc/user.h"
#include "crow_all.h"

#define TUC_VERSION "0.1 Alpha"
#define TUC_COMMIT "?" // 

class shell {
    private:
        crow::SimpleApp crow_app;
    public:
        void start() {
            crow_app.port(18080).multithreaded().run();
        }
        void start(int port) {
            crow_app.port(port).multithreaded().run();
        }
        void start(std::string ip, int port); // ???
};