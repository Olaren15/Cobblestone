#include <filesystem>

#include <iostream>

#include <libconfig.h++>

namespace flex {
class Configuration {
private:
  libconfig::Config mConfigSettings;

  libconfig::Setting &
  loadConfigFile(const std::filesystem::path &configFilePath);

public:
  Configuration();
  Configuration(const std::filesystem::path &configFilePath);
  ~Configuration();

  int getInt(const std::string &key) const;
  unsigned int getUsignedInt(const std::string &key) const;
  float getFloat(const std::string &key) const;
  double getDouble(const std::string &key) const;
  bool getBool(const std::string &key) const;
  std::string getString(const std::string &key) const;
};
} // namespace flex