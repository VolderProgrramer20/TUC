#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <filesystem>

bool file_exist(std::filesystem::path file_path) {
	std::ifstream file(file_path, std::ios::in);
	if (file) {
		file.close();
		return true;
	}
	else { return false; }
}