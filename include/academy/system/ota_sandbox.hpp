#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace academy::system {

// A small, real SHA-256 implementation so the host OTA exercise verifies the
// bytes it stages. A digest proves integrity only; production OTA also needs
// a trusted signature and ESP32 secure-boot/flash-encryption configuration.
class Sha256 {
public:
    Sha256() { reset(); }

    void update(const std::uint8_t* data, std::size_t length) {
        for (std::size_t i = 0; i < length; ++i) {
            block_[used_++] = data[i];
            if (used_ == block_.size()) {
                transform();
                bits_ += 512;
                used_ = 0;
            }
        }
    }

    std::string final_hex() {
        const std::uint64_t total_bits = bits_ + static_cast<std::uint64_t>(used_) * 8;
        block_[used_++] = 0x80;
        if (used_ > 56) {
            while (used_ < 64) block_[used_++] = 0;
            transform();
            used_ = 0;
        }
        while (used_ < 56) block_[used_++] = 0;
        for (int shift = 56; shift >= 0; shift -= 8) block_[used_++] = static_cast<std::uint8_t>(total_bits >> shift);
        transform();
        std::ostringstream out;
        for (auto word : state_) out << std::hex << std::setfill('0') << std::setw(8) << word;
        return out.str();
    }

    static std::string file_hex(const std::filesystem::path& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in) throw std::runtime_error("无法读取固件包: " + path.string());
        Sha256 hash;
        std::array<char, 4096> buffer{};
        while (in.read(buffer.data(), buffer.size()) || in.gcount())
            hash.update(reinterpret_cast<const std::uint8_t*>(buffer.data()), static_cast<std::size_t>(in.gcount()));
        return hash.final_hex();
    }

private:
    static constexpr std::array<std::uint32_t, 64> k_ = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};
    static std::uint32_t rotr(std::uint32_t value, unsigned bits) { return (value >> bits) | (value << (32 - bits)); }
    static std::uint32_t choose(std::uint32_t x, std::uint32_t y, std::uint32_t z) { return (x & y) ^ (~x & z); }
    static std::uint32_t majority(std::uint32_t x, std::uint32_t y, std::uint32_t z) { return (x & y) ^ (x & z) ^ (y & z); }
    void reset() { state_ = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19}; bits_ = 0; used_ = 0; }
    void transform() {
        std::array<std::uint32_t, 64> words{};
        for (int i = 0; i < 16; ++i) words[i] = (static_cast<std::uint32_t>(block_[i * 4]) << 24) | (static_cast<std::uint32_t>(block_[i * 4 + 1]) << 16) | (static_cast<std::uint32_t>(block_[i * 4 + 2]) << 8) | block_[i * 4 + 3];
        for (int i = 16; i < 64; ++i) {
            const auto s0 = rotr(words[i - 15], 7) ^ rotr(words[i - 15], 18) ^ (words[i - 15] >> 3);
            const auto s1 = rotr(words[i - 2], 17) ^ rotr(words[i - 2], 19) ^ (words[i - 2] >> 10);
            words[i] = words[i - 16] + s0 + words[i - 7] + s1;
        }
        auto a = state_[0], b = state_[1], c = state_[2], d = state_[3], e = state_[4], f = state_[5], g = state_[6], h = state_[7];
        for (int i = 0; i < 64; ++i) {
            const auto s1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
            const auto temp1 = h + s1 + choose(e, f, g) + k_[i] + words[i];
            const auto s0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
            const auto temp2 = s0 + majority(a, b, c);
            h = g; g = f; f = e; e = d + temp1; d = c; c = b; b = a; a = temp1 + temp2;
        }
        state_[0] += a; state_[1] += b; state_[2] += c; state_[3] += d;
        state_[4] += e; state_[5] += f; state_[6] += g; state_[7] += h;
    }
    std::array<std::uint32_t, 8> state_{};
    std::array<std::uint8_t, 64> block_{};
    std::uint64_t bits_ = 0;
    std::size_t used_ = 0;
};

struct OtaResult { bool ok; std::string state; std::string detail; };

class OtaSandbox {
public:
    explicit OtaSandbox(std::filesystem::path root) : root_(std::move(root)) { initialize(); }

    OtaResult stage(const std::filesystem::path& package) {
        const auto manifest = package / "manifest.txt";
        const auto image = package / "firmware.bin";
        if (!std::filesystem::is_regular_file(manifest) || !std::filesystem::is_regular_file(image))
            return {false, "rejected", "包必须含 manifest.txt 和 firmware.bin"};
        const auto values = read_manifest(manifest);
        const auto version = get(values, "version");
        const auto claimed_hash = get(values, "sha256");
        if (version.empty() || claimed_hash.size() != 64) return {false, "rejected", "manifest 缺少 version 或 sha256"};
        const auto actual_hash = Sha256::file_hex(image);
        if (actual_hash != claimed_hash) return {false, "rejected", "SHA-256 不匹配，未写入候选槽"};
        const auto candidate = root_ / "staging";
        std::filesystem::remove_all(candidate);
        std::filesystem::create_directories(candidate);
        std::filesystem::copy_file(image, candidate / "firmware.bin", std::filesystem::copy_options::overwrite_existing);
        write_text(candidate / "version.txt", version);
        return {true, "verified", "包校验成功，等待激活"};
    }

    OtaResult activate() {
        const auto staging = root_ / "staging";
        if (!std::filesystem::is_regular_file(staging / "firmware.bin")) return {false, "rejected", "没有已校验的候选包"};
        const auto active = read_text(root_ / "current.txt");
        const auto candidate_slot = active == "A" ? "B" : "A";
        const auto slot = root_ / "slots" / candidate_slot;
        std::filesystem::remove_all(slot);
        std::filesystem::create_directories(slot);
        std::filesystem::copy_file(staging / "firmware.bin", slot / "firmware.bin", std::filesystem::copy_options::overwrite_existing);
        write_text(slot / "version.txt", read_text(staging / "version.txt"));
        write_text(root_ / "pending.txt", candidate_slot);
        return {true, "pending_boot", "候选槽已准备；下次启动必须完成健康检查"};
    }

    OtaResult boot(bool health_check_passed) {
        const auto pending = root_ / "pending.txt";
        if (!std::filesystem::exists(pending)) return {true, "running", "当前固件继续运行"};
        const auto candidate = read_text(pending);
        if (health_check_passed) {
            write_text(root_ / "current.txt", candidate);
            std::filesystem::remove(pending);
            return {true, "confirmed", "健康检查通过，已确认新固件"};
        }
        std::filesystem::remove_all(root_ / "slots" / candidate);
        std::filesystem::remove(pending);
        return {false, "rolled_back", "健康检查失败，保留原槽并丢弃候选槽"};
    }

    std::string current_slot() const { return read_text(root_ / "current.txt"); }

private:
    void initialize() {
        std::filesystem::create_directories(root_ / "slots" / "A");
        std::filesystem::create_directories(root_ / "slots" / "B");
        if (!std::filesystem::exists(root_ / "current.txt")) write_text(root_ / "current.txt", "A");
    }
    static std::map<std::string, std::string> read_manifest(const std::filesystem::path& path) {
        std::ifstream in(path);
        std::map<std::string, std::string> values;
        for (std::string line; std::getline(in, line);) {
            const auto split = line.find('=');
            if (split != std::string::npos) values[line.substr(0, split)] = line.substr(split + 1);
        }
        return values;
    }
    static std::string get(const std::map<std::string, std::string>& values, const std::string& key) {
        const auto found = values.find(key); return found == values.end() ? "" : found->second;
    }
    static std::string read_text(const std::filesystem::path& path) {
        std::ifstream in(path); std::string value; std::getline(in, value); return value;
    }
    static void write_text(const std::filesystem::path& path, const std::string& value) {
        std::filesystem::create_directories(path.parent_path());
        const auto temp = path.string() + ".tmp";
        { std::ofstream out(temp, std::ios::trunc); if (!out) throw std::runtime_error("无法写状态文件"); out << value << '\n'; }
        std::error_code error;
        std::filesystem::rename(temp, path, error);
        if (error) { std::filesystem::remove(path, error); std::filesystem::rename(temp, path, error); }
        if (error) throw std::runtime_error("无法替换状态文件: " + error.message());
    }
    std::filesystem::path root_;
};

}  // namespace academy::system
