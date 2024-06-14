#include <string>

#define TUC_VERSION "0.1 Alpha"
#define TUC_COMMIT "?" // 

class shell {
    private:
    public:
        void start();
        void start(int port);
        void start(std::string ip, int port);
};