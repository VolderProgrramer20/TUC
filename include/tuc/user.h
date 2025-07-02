#include <string>

enum class permissions_enum {
        ghost = 0,
        user,
        admin
};

class user final {
    private:
        permissions_enum permissions;
        std::string name, password, hashed_password, language, key;
        int user_id;
        bool user_сreated = false;
    public:
        user() {}
        user(std::string name, permissions_enum permissions, int user_id) {
            if (!user_сreated) {
                this->name = name;
                this->permissions = permissions;
                this->user_id = user_id;
                this->hashed_password = password; // TODO: Use SHA-256 algorithm for password hashing...? 🥶
                user_сreated = true;
            }
        }
        bool true_password(std::string password) { return password == this->password; }
        // Пользователь не имеет нужных разрешений*
        bool user_has_no_permissions(permissions_enum permissions) { return permissions > this->permissions;}
        inline std::string get_hashed_password() { return this->hashed_password; }
        inline std::string get_name() { return this->name; }
        inline permissions_enum get_permissions() { return this->permissions; }
        inline int get_user_id() { return this->user_id; }
};
