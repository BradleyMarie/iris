#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#ifdef __linux__
#include <sys/ioctl.h>
#endif

#ifdef _XOPEN_SOURCE
#include <unistd.h>
#endif

#ifdef INSTRUMENTED_BUILD
#include <gperftools/profiler.h>
#endif  // INSTRUMENTED_BUILD

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/flags/usage_config.h"
#include "frontends/pbrt/parser.h"
#include "frontends/pbrt/spectrum_managers/color_spectrum_manager.h"
#include "frontends/pbrt/tokenizer.h"
#include "iris/color_matchers/cie_color_matcher.h"
#include "iris/random/mersenne_twister_random.h"

#ifdef INSTRUMENTED_BUILD
ABSL_FLAG(
    std::string, cpu_profile, "",
    "If non-empty, enable CPU profiling and save traces to the file specified. "
    "If empty, CPU profiling is disabled and no output is generated.");
#endif  // INSTRUMENTED_BUILD

ABSL_FLAG(
    bool, all_spectra_are_reflective, true,
    "If true, reproduce a bug in PBRT where the reflective CIE color matching "
    "function is always used to convert sampled spectra to XYZ colors.");

ABSL_FLAG(iris::geometric_t, epsilon, 0.001,
          "The amount of error tolerated in distance calculations. Must be "
          "finite and greater than or equal to zero.");

#ifdef _XOPEN_SOURCE
ABSL_FLAG(unsigned short, nice_increment, 19,
          "The number of steps to increment the nice value of iris.");
#endif

ABSL_FLAG(unsigned int, num_threads, 0,
          "The number of threads to use for rendering. If zero, the "
          "number of threads will equal the number of processors in the "
          "system.");

ABSL_FLAG(bool, spectral, false, "If true, spectral rendering is performed.");

namespace {

#if defined NDEBUG
static const std::string kDebug;
#else
static const std::string kDebug = " debug";
#endif  // defined NDEBUG

static const std::string kVersion = "0.1";
static const std::string kBits = (sizeof(void*) == 4) ? "32-bit" : "64-bit";
static_assert(sizeof(void*) == 4 || sizeof(void*) == 8);

static size_t kDefaultProgressWidth = 80;
static size_t kReservedSuffixSpace = 16;

std::string VersionString() {
  std::stringstream result;
  result << "iris " << kVersion << " (" << kBits << kDebug << ") ["
         << std::thread::hardware_concurrency() << " hardware threads detected]"
         << std::endl;
  return result.str();
}

static const absl::FlagsUsageConfig flags_usage_config = {
    nullptr, nullptr, nullptr, VersionString, nullptr};

}  // namespace

int main(int argc, char** argv) {
  absl::SetProgramUsageMessage(
      "A pbrt frontend for the iris renderer\n\nUsage: iris [options] file");
  absl::SetFlagsUsageConfig(flags_usage_config);

  auto unparsed = absl::ParseCommandLine(argc, argv);
  if (2 < unparsed.size()) {
    std::cerr << "ERROR: Only one file input supported";
    return EXIT_FAILURE;
  }

  if (absl::GetFlag(FLAGS_epsilon) < 0.0 ||
      !std::isfinite(absl::GetFlag(FLAGS_epsilon))) {
    std::cerr
        << "ERROR: epsilon must be finite and greater than or equal to zero"
        << std::endl;
    exit(EXIT_FAILURE);
  }

  if (absl::GetFlag(FLAGS_num_threads) == 0) {
    absl::SetFlag(&FLAGS_num_threads, std::thread::hardware_concurrency());
  }

  std::unique_ptr<iris::pbrt_frontend::SpectrumManager> spectral_manager;
  std::unique_ptr<iris::ColorMatcher> color_matcher;
  if (absl::GetFlag(FLAGS_spectral)) {
    spectral_manager = nullptr;
    color_matcher = std::make_unique<iris::color_matchers::CieColorMatcher>();

    std::cerr << "ERROR: Spectral rendering is not yet implemented"
              << std::endl;
    exit(EXIT_FAILURE);
  } else {
    spectral_manager = std::make_unique<
        iris::pbrt_frontend::spectrum_managers::ColorSpectrumManager>(
        absl::GetFlag(FLAGS_all_spectra_are_reflective));
    color_matcher = std::make_unique<
        iris::pbrt_frontend::spectrum_managers::ColorColorMatcher>();
  }

#ifdef _XOPEN_SOURCE
  if (nice(absl::GetFlag(FLAGS_nice_increment)) == -1) {
    std::cerr << "WARNING: Failed to lower the adjust of the process"
              << std::endl;
  }
#endif

#ifdef INSTRUMENTED_BUILD
  const auto& cpu_profile = absl::GetFlag(FLAGS_cpu_profile);
  if (!cpu_profile.empty()) {
    ProfilerStart(cpu_profile.c_str());
  }
#endif  // INSTRUMENTED_BUILD

  std::unique_ptr<iris::pbrt_frontend::Tokenizer> tokenizer;
  std::unique_ptr<std::ifstream> file_input;
  std::filesystem::path search_root;
  if (unparsed.size() == 1) {
    tokenizer = std::make_unique<iris::pbrt_frontend::Tokenizer>(std::cin);
    search_root = std::filesystem::current_path();
  } else {
    file_input = std::make_unique<std::ifstream>(unparsed.at(1));
    if (file_input->fail()) {
      std::cerr << "ERROR: Failed to open file: " << unparsed.at(1)
                << std::endl;
      exit(EXIT_FAILURE);
    }

    auto file_path = std::filesystem::weakly_canonical(unparsed.at(1));
    tokenizer = std::make_unique<iris::pbrt_frontend::Tokenizer>(*file_input,
                                                                 file_path);
    search_root = file_path.parent_path();
  }

  iris::pbrt_frontend::Parser parser(std::move(spectral_manager));
  for (size_t render_index = 0;; render_index += 1) {
    auto result = parser.ParseFrom(*tokenizer, search_root);
    if (!result) {
      if (render_index == 0) {
        std::cerr << "ERROR: Render input was empty." << std::endl;
        exit(EXIT_FAILURE);
      }

      break;
    }

    bool first = true;
    auto start_time = std::chrono::steady_clock::now();
    auto progress_callback = [&](size_t current_pixel, size_t num_pixels) {
      auto current_time = std::chrono::steady_clock::now();

      std::string prefix;
      if (unparsed.size() == 1) {
        prefix = "Rendering (" + std::to_string(render_index + 1) + ") [";
      } else {
        prefix = "Rendering [";
      }

      std::chrono::duration<float> elapsed_time(current_time - start_time);
      float chunks_per_second = static_cast<float>(current_pixel) /
                                static_cast<float>(elapsed_time.count());
      int elapsed_time_seconds = elapsed_time.count();
      int estimated_time_remaining_seconds =
          static_cast<float>(num_pixels - current_pixel) / chunks_per_second;

      std::string suffix = "] (";
      if (current_pixel == num_pixels) {
        suffix += std::to_string(elapsed_time_seconds) + "s)";
      } else if (first) {
        suffix += std::to_string(elapsed_time_seconds) + "s|?s)";
      } else {
        suffix += std::to_string(elapsed_time_seconds) + "s|" +
                  std::to_string(estimated_time_remaining_seconds) + "s)";
      }

      while (suffix.size() < kReservedSuffixSpace) {
        suffix.push_back(' ');
      }

      if (current_pixel == num_pixels) {
        suffix.push_back('\n');
      } else {
        suffix.push_back('\r');
      }

      size_t text_width = kDefaultProgressWidth;

#ifdef __linux__
      struct winsize window;
      int result = ioctl(STDOUT_FILENO, TIOCGWINSZ, &window);
      if (result >= 0) {
        text_width = window.ws_col;
      }
#endif

      size_t bar_width = text_width - prefix.size() - suffix.size();
      float progress =
          static_cast<float>(current_pixel) / static_cast<float>(num_pixels);
      size_t bar_length = bar_width * progress;

      std::string bar;
      for (size_t i = 0; i < bar_width; i++) {
        if (i < bar_length) {
          bar += "=";
        } else if (i == bar_length) {
          bar += ">";
        } else {
          bar += " ";
        }
      }

      std::cout << prefix << bar << suffix << std::flush;

      first = false;
    };

    std::ofstream output(result->output_filename, std::ofstream::out |
                                                      std::ofstream::binary |
                                                      std::ofstream::trunc);

    iris::random::MersenneTwisterRandom rng;  // TODO: Support other RNG
    auto framebuffer = result->renderable.Render(
        *color_matcher, rng, absl::GetFlag(FLAGS_epsilon),
        absl::GetFlag(FLAGS_num_threads), progress_callback);

    result->output_write_function(framebuffer, output);
  }

#ifdef INSTRUMENTED_BUILD
  ProfilerStop();
#endif  // INSTRUMENTED_BUILD

  return EXIT_SUCCESS;
}