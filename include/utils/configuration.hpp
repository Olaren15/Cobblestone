#pragma once

#include <filesystem>

#include <iostream>

#include <libconfig.h++>

namespace flex {
class Configuration {
private:
  libconfig::Config mConfigSettings;

public:
  Configuration();
  Configuration(std::filesystem::path const &configFilePath);
  ~Configuration();

  int getInt(std::string const &key) const;
  unsigned int getUsignedInt(std::string const &key) const;
  float getFloat(std::string const &key) const;
  double getDouble(std::string const &key) const;
  bool getBool(std::string const &key) const;
  std::string getString(std::string const &key) const;
};
} // namespace flex