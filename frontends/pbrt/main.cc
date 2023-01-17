#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

#ifdef INSTRUMENTED_BUILD
#include <gperftools/profiler.h>
#endif  // INSTRUMENTED_BUILD

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/flags/usage_config.h"
#include "frontends/pbrt/parser.h"
#include "frontends/pbrt/tokenizer.h"
#include "iris/random/mersenne_twister_random.h"

#ifdef INSTRUMENTED_BUILD
ABSL_FLAG(
    std::string, cpu_profile, "",
    "If non-empty, enable CPU profiling and save traces to the file specified. "
    "If empty, CPU profiling is disabled and no output is generated.");
#endif  // INSTRUMENTED_BUILD

ABSL_FLAG(iris::geometric_t, epsilon, 0.001,
          "The amount of error tolerated in distance calculations. Must be "
          "finite and greater than or equal to zero.");

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
    color_matcher = nullptr;
  } else {
    spectral_manager = nullptr;
    color_matcher = nullptr;
  }

#ifdef INSTRUMENTED_BUILD
  const auto& cpu_profile = absl::GetFlag(FLAGS_cpu_profile);
  if (!cpu_profile.empty()) {
    ProfilerStart(cpu_profile.c_str());
  }
#endif  // INSTRUMENTED_BUILD

  std::unique_ptr<iris::pbrt_frontend::Tokenizer> tokenizer;
  std::unique_ptr<std::ifstream> file_input;
  if (unparsed.size() == 1) {
    tokenizer = std::make_unique<iris::pbrt_frontend::Tokenizer>(std::cin);
  } else {
    file_input = std::make_unique<std::ifstream>(unparsed.at(0));
    tokenizer = std::make_unique<iris::pbrt_frontend::Tokenizer>(*file_input);
  }

  iris::pbrt_frontend::Parser parser(std::move(spectral_manager));
  for (size_t render_index = 0;; render_index += 1) {
    auto result = parser.ParseFrom(".", *tokenizer, ".");
    if (!result) {
      if (render_index == 0) {
        std::cerr << "ERROR: Render input was empty." << std::endl;
        exit(EXIT_FAILURE);
      }

      break;
    }

    std::ofstream output(result->output_filename, std::ofstream::out |
                                                      std::ofstream::binary |
                                                      std::ofstream::trunc);

    iris::random::MersenneTwisterRandom rng;  // TODO: Support other RNG
    auto framebuffer = result->renderable.Render(
        *color_matcher, rng, absl::GetFlag(FLAGS_epsilon),
        absl::GetFlag(FLAGS_num_threads));

    result->output_write_function(framebuffer, output);
  }

#ifdef INSTRUMENTED_BUILD
  ProfilerStop();
#endif  // INSTRUMENTED_BUILD

  return EXIT_SUCCESS;
}