#ifndef CASM_monte_CompletionCheck
#define CASM_monte_CompletionCheck

#include <optional>

#include "casm/monte/checks/ConvergenceCheck.hh"
#include "casm/monte/checks/CutoffCheck.hh"
#include "casm/monte/checks/EquilibrationCheck.hh"
#include "casm/monte/definitions.hh"
#include "casm/monte/sampling/Sampler.hh"

namespace CASM {
namespace monte {

// --- Completion checking (cutoff & convergence) ---

/// \brief Parameters that determine if a calculation is complete
struct CompletionCheckParams {
  /// \brief Completion check parameters that don't depend on the sampled values
  CutoffCheckParams cutoff_params;

  /// \brief Sampler components that must be checked for convergence, and the
  ///     estimated precision to which the mean must be converged
  std::map<SamplerComponent, double> requested_precision;

  /// \brief Confidence level for calculated precision of mean
  double confidence = 0.95;

  /// \brief Minimum number of samples before checking for completion
  CountType check_begin = 10;

  /// \brief How often to check for completion
  ///
  /// Check for completion performed if:
  /// - n_samples % check_frequency == 0 && n_samples >= check_begin
  CountType check_frequency = 1;
};

/// \brief Stores completion check results
struct CompletionCheckResults {
  /// True if calculation is complete, either due to convergence or cutoff
  bool is_complete = false;

  EquilibrationCheckResults equilibration_check_results;

  /// \brief Confidence level used for calculated precision of mean
  double confidence = 0.95;

  ConvergenceCheckResults convergence_check_results;
};

/// \brief Checks if a cutoff or convergence criteria are met
class CompletionCheck {
 public:
  CompletionCheck(CompletionCheckParams params);

  bool is_complete(
      std::map<std::string, std::shared_ptr<Sampler>> const &samplers);

  bool is_complete(
      std::map<std::string, std::shared_ptr<Sampler>> const &samplers,
      CountType count);

  bool is_complete(
      std::map<std::string, std::shared_ptr<Sampler>> const &samplers,
      TimeType time);

  bool is_complete(
      std::map<std::string, std::shared_ptr<Sampler>> const &samplers,
      CountType count, TimeType time);

  CompletionCheckResults const &results() const { return m_results; }

 private:
  bool _is_complete(
      std::map<std::string, std::shared_ptr<Sampler>> const &samplers,
      std::optional<CountType> count, std::optional<TimeType> time);

  void _check(std::map<std::string, std::shared_ptr<Sampler>> const &samplers,
              std::optional<CountType> count, std::optional<TimeType> time,
              CountType n_samples);

  CompletionCheckParams m_params;

  CompletionCheckResults m_results;
};

// --- Inline definitions ---

inline bool CompletionCheck::is_complete(
    std::map<std::string, std::shared_ptr<Sampler>> const &samplers) {
  return _is_complete(samplers, std::nullopt, std::nullopt);
}

inline bool CompletionCheck::is_complete(
    std::map<std::string, std::shared_ptr<Sampler>> const &samplers,
    CountType count) {
  return _is_complete(samplers, count, std::nullopt);
}

inline bool CompletionCheck::is_complete(
    std::map<std::string, std::shared_ptr<Sampler>> const &samplers,
    TimeType time) {
  return _is_complete(samplers, std::nullopt, time);
}

inline bool CompletionCheck::is_complete(
    std::map<std::string, std::shared_ptr<Sampler>> const &samplers,
    CountType count, TimeType time) {
  return _is_complete(samplers, count, time);
}

inline bool CompletionCheck::_is_complete(
    std::map<std::string, std::shared_ptr<Sampler>> const &samplers,
    std::optional<CountType> count, std::optional<TimeType> time) {
  CountType n_samples = get_n_samples(samplers);
  // if all minimums not met, continue
  if (!all_minimums_met(m_params.cutoff_params, count, time, n_samples)) {
    return false;
  }
  if (n_samples >= m_params.check_begin &&
      n_samples % m_params.check_frequency == 0) {
    _check(samplers, count, time, n_samples);
  }
  // if any maximum met, stop
  if (any_maximum_met(m_params.cutoff_params, count, time, n_samples)) {
    m_results = CompletionCheckResults();
    m_results.confidence = m_params.confidence;
    m_results.is_complete = true;
  }
  return m_results.is_complete;
}

}  // namespace monte
}  // namespace CASM

#endif
