#pragma once

#include <filesystem>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>

namespace academy::system {

struct Config {
    int version = 2;
    std::string device_name = "desk-notifier";
    int report_interval_ms = 60000;
};

struct LoadResult {
    Config config;
    bool migrated = false;
};

class ConfigStore {
public:
    explicit ConfigStore(std::filesystem::path directory)
        : directory_(std::move(directory)), path_(directory_ / "device.conf") {}

    LoadResult load() {
        if (!std::filesystem::exists(path_)) return {};
        const auto values = read_values(path_);
        const int version = to_int(values, "version", 1);
        if (version == 1) {
            Config migrated;
            migrated.device_name = get(values, "name", migrated.device_name);
            migrated.report_interval_ms = to_int(values, "interval_s", 60) * 1000;
            validate(migrated);
            save(migrated);
            return {migrated, true};
        }
        if (version != 2) throw std::runtime_error("不支持的配置版本: " + std::to_string(version));
        Config config;
        config.device_name = get(values, "device_name", config.device_name);
        config.report_interval_ms = to_int(values, "report_interval_ms", config.report_interval_ms);
        validate(config);
        return {config, false};
    }

    void save(const Config& config) const {
        validate(config);
        std::filesystem::create_directories(directory_);
        const auto temporary = directory_ / "device.conf.tmp";
        {
            std::ofstream out(temporary, std::ios::trunc);
            if (!out) throw std::runtime_error("无法写入临时配置文件");
            out << "version=2\n"
                << "device_name=" << config.device_name << "\n"
                << "report_interval_ms=" << config.report_interval_ms << "\n";
            out.flush();
            if (!out) throw std::runtime_error("写入配置失败");
        }
        // Keep the temporary file in the target directory. On ESP32 this role is
        // fulfilled by NVS transactions, not this host file implementation.
        std::error_code error;
        std::filesystem::rename(temporary, path_, error);
        if (error) {
            std::filesystem::remove(path_, error);
            std::filesystem::rename(temporary, path_, error);
        }
        if (error) throw std::runtime_error("无法替换配置文件: " + error.message());
    }

    const std::filesystem::path& path() const { return path_; }

private:
    static std::map<std::string, std::string> read_values(const std::filesystem::path& path) {
        std::ifstream in(path);
        if (!in) throw std::runtime_error("无法读取配置文件");
        std::map<std::string, std::string> values;
        for (std::string line; std::getline(in, line);) {
            const auto split = line.find('=');
            if (split == std::string::npos) continue;
            values[line.substr(0, split)] = line.substr(split + 1);
        }
        return values;
    }

    static std::string get(const std::map<std::string, std::string>& values,
                           const std::string& key, const std::string& fallback) {
        const auto found = values.find(key);
        return found == values.end() ? fallback : found->second;
    }

    static int to_int(const std::map<std::string, std::string>& values,
                      const std::string& key, int fallback) {
        const auto found = values.find(key);
        if (found == values.end()) return fallback;
        try {
            size_t parsed = 0;
            const int value = std::stoi(found->second, &parsed);
            if (parsed != found->second.size()) throw std::runtime_error("tail");
            return value;
        } catch (...) {
            throw std::runtime_error("配置项不是整数: " + key);
        }
    }

    static void validate(const Config& config) {
        if (config.device_name.empty() || config.device_name.size() > 32)
            throw std::runtime_error("device_name 必须为 1 到 32 个字符");
        if (config.report_interval_ms < 1000 || config.report_interval_ms > 3600000)
            throw std::runtime_error("report_interval_ms 必须在 1000 到 3600000 之间");
    }

    std::filesystem::path directory_;
    std::filesystem::path path_;
};

}  // namespace academy::system
