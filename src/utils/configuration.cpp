#include "utils/configuration.hpp"

namespace flex {
Configuration::Configuration() { mConfigSettings.readFile("settings.cfg"); }

Configuration::Configuration(std::filesystem::path const &configFilePath) {
  mConfigSettings.readFile(configFilePath.string().c_str());
}

Configuration::~Configuration() = default;

int Configuration::getInt(std::string const &key) const {
  int value = 0;

  try {
    value = mConfigSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

unsigned int Configuration::getUnsignedInt(std::string const &key) const {
  unsigned int value = 0u;

  try {
    value = mConfigSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

float Configuration::getFloat(std::string const &key) const {
  float value = 0.0f;

  try {
    value = mConfigSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

double Configuration::getDouble(std::string const &key) const {
  double value = 0.0L;

  try {
    value = mConfigSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

bool Configuration::getBool(std::string const &key) const {
  bool value = false;

  try {
    value = mConfigSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

std::string Configuration::getString(std::string const &key) const {
  std::string value;

  try {
    value = static_cast<const char *>(mConfigSettings.lookup(key));
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}
} // namespace flex