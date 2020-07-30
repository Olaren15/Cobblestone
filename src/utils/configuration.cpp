#include "utils/configuration.hpp"

namespace flex {
Configuration::Configuration() { mConfigSettings.readFile("settings.cfg"); }

Configuration::Configuration(const std::filesystem::path &configFilePath) {
  mConfigSettings.readFile(configFilePath.c_str());
}

Configuration::~Configuration() {}

int Configuration::getInt(const std::string &key) const {
  int value = 0;

  try {
    value = mConfigSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

unsigned int Configuration::getUsignedInt(const std::string &key) const {
  unsigned int value = 0u;

  try {
    value = mConfigSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

float Configuration::getFloat(const std::string &key) const {
  float value = 0.0f;

  try {
    value = mConfigSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

double Configuration::getDouble(const std::string &key) const {
  double value = 0.0L;

  try {
    value = mConfigSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

bool Configuration::getBool(const std::string &key) const {
  bool value = false;

  try {
    value = mConfigSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

std::string Configuration::getString(const std::string &key) const {
  std::string value = "";

  try {
    value = (const char *)mConfigSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

} // namespace flex