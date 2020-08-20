#pragma once

#include <filesystem>

#pragma warning(push, 0)
#include <libconfig.h++>
#pragma warning(pop)

namespace flex {
class Configuration {
private:
  libconfig::Config mConfigSettings;

public:
  Configuration();
  explicit Configuration(std::filesystem::path const &configFilePath);
  ~Configuration();

  [[nodiscard]] int getInt(std::string const &key) const;
  [[nodiscard]] unsigned int getUnsignedInt(std::string const &key) const;
  [[nodiscard]] float getFloat(std::string const &key) const;
  [[nodiscard]] double getDouble(std::string const &key) const;
  [[nodiscard]] bool getBool(std::string const &key) const;
  [[nodiscard]] std::string getString(std::string const &key) const;
};
} // namespace flex
