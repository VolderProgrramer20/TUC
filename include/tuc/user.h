#include <string>

class user final {
    private:
        std::string username, password, language, key, permissions;
        int user_id;
        bool userCreated = false;
    public:
        user(std::string username, std::string permissions, int user_id) {
            if (!userCreated) {
                this->username = username;
                this->permissions = permissions;
                this->user_id = user_id;
                userCreated = true;
            }
        }
        bool true_password(std::string password) { return password == this->password; }
        inline std::string get_username() { return this->username; }
        inline std::string get_permissions() { return this->permissions; }
        inline int get_user_id() { return this->user_id; }
};