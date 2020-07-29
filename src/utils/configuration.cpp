#include "utils/configuration.hpp"

namespace flex {
Configuration::Configuration()
    : mRootSettings(loadConfigFile("settings.cfg")) {}

Configuration::Configuration(const std::filesystem::path &configFilePath)
    : mRootSettings(loadConfigFile(configFilePath)) {}

Configuration::~Configuration() {}

libconfig::Setting &
Configuration::loadConfigFile(const std::filesystem::path &configFilePath) {
  libconfig::Config configFile;

  configFile.readFile(configFilePath.c_str());

  return configFile.getRoot();
}

int Configuration::getInt(const std::string &key) {
  int value = 0;

  try {
    value = mRootSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

unsigned int Configuration::getUsignedInt(const std::string &key) {
  unsigned int value = 0u;

  try {
    value = mRootSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

float Configuration::getFloat(const std::string &key) {
  float value = 0.0f;

  try {
    value = mRootSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

double Configuration::getDouble(const std::string &key) {
  double value = 0.0L;

  try {
    value = mRootSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

bool Configuration::getBool(const std::string &key) {
  bool value = false;

  try {
    value = mRootSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

std::string Configuration::getString(const std::string &key) {
  std::string value = "";

  try {
    value = (const char *)mRootSettings.lookup(key);
  } catch (...) {
    // return default value previously assigned
  }

  return value;
}

} // namespace flex