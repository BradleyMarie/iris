#ifndef _FRONTENDS_PBRT_TOKENIZER_
#define _FRONTENDS_PBRT_TOKENIZER_

#include <cassert>
#include <filesystem>
#include <istream>
#include <optional>
#include <string_view>

namespace iris::pbrt_frontend {

class Tokenizer {
 public:
  Tokenizer(std::istream& stream) noexcept
      : stream_(&stream), search_root_(std::filesystem::current_path()) {}

  Tokenizer(std::istream& stream,
            const std::filesystem::path& file_path) noexcept
      : stream_(&stream),
        file_path_(file_path),
        search_root_(file_path.parent_path()) {
    assert(std::filesystem::is_regular_file(file_path));
    assert(std::filesystem::exists(file_path));
    assert(std::filesystem::weakly_canonical(file_path) == file_path);
  }

  Tokenizer(Tokenizer&& moved_from) noexcept;
  Tokenizer& operator=(Tokenizer&& moved_from) noexcept;

  Tokenizer(const Tokenizer&) = delete;
  Tokenizer& operator=(const Tokenizer&) = delete;

  std::optional<std::string_view> Peek();
  std::optional<std::string_view> Next();

  const std::filesystem::path& SearchRoot() const { return search_root_; }
  const std::optional<std::filesystem::path>& FilePath() const {
    return file_path_;
  }

 private:
  bool ParseNext(std::string& output);

  std::istream* stream_;
  std::optional<std::filesystem::path> file_path_;
  std::filesystem::path search_root_;
  std::string next_;
  std::string peeked_;
  std::optional<bool> peeked_valid_;
};

}  // namespace iris::pbrt_frontend

#endif  // _IRIS_FRONTEND_TOKENIZER_