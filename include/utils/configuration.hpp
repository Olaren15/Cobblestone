#include <filesystem>

#include <libconfig.h++>

namespace flex {
class Configuration {
private:
  libconfig::Setting &mRootSettings;

  libconfig::Setting &
  loadConfigFile(const std::filesystem::path &configFilePath);

public:
  Configuration();
  Configuration(const std::filesystem::path &configFilePath);
  ~Configuration();

  int getInt(const std::string &key);
  unsigned int getUsignedInt(const std::string &key);
  float getFloat(const std::string &key);
  double getDouble(const std::string &key);
  bool getBool(const std::string &key);
  std::string getString(const std::string &key);
};
} // namespace flex