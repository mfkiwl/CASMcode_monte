#include "casm/monte/events/io/OccCandidate_stream_io.hh"

#include "casm/crystallography/UnitCellCoord.hh"
#include "casm/monte/Conversions.hh"
#include "casm/monte/events/OccCandidate.hh"

namespace CASM {

std::ostream &operator<<(
    std::ostream &sout,
    std::pair<monte::OccCandidate const &, monte::Conversions const &> value) {
  sout << "(" << value.second.species_name(value.first.species_index) << ", "
       << value.first.asym << ")";
  return sout;
}

std::ostream &operator<<(
    std::ostream &sout,
    std::pair<monte::OccSwap const &, monte::Conversions const &> value) {
  using namespace monte;
  sout << std::pair<OccCandidate const &, Conversions const &>(
              value.first.cand_a, value.second)
       << " <-> "
       << std::pair<OccCandidate const &, Conversions const &>(
              value.first.cand_b, value.second);
  return sout;
}

std::ostream &operator<<(
    std::ostream &sout,
    std::pair<monte::OccCandidateList const &, monte::Conversions const &>
        value) {
  using namespace monte;
  typedef std::pair<OccCandidate const &, Conversions const &> cand_pair;
  typedef std::pair<OccSwap const &, Conversions const &> swap_pair;
  Conversions const &convert = value.second;
  OccCandidateList const &list = value.first;

  sout << "Unit cell for determining equivalent swaps: \n"
       << convert.unit_transformation_matrix_to_super() << "\n\n";

  sout << "Asymmetric Unit: " << std::endl;
  for (Index asym = 0; asym != convert.asym_size(); ++asym) {
    sout << "  " << asym << ": ";
    for (Index i = 0; i != convert.occ_size(asym); ++i) {
      sout << convert.species_name(convert.species_index(asym, i)) << " ";
    }
    sout << "\n";

    const auto &set = convert.asym_to_unitl(asym);
    for (auto it = set.begin(); it != set.end(); ++it) {
      sout << "    " << convert.unitl_to_bijk(*it) << "\n";
    }
  }
  sout << "\n";

  sout << "Candidates: (Species, AsymUnit)" << std::endl;
  for (auto it = list.begin(); it != list.end(); ++it) {
    sout << "  " << cand_pair(*it, convert) << "\n";
  }
  sout << "\n";

  sout << "Canonical swaps: " << std::endl;
  for (auto it = list.canonical_swap().begin();
       it != list.canonical_swap().end(); ++it) {
    sout << "  " << swap_pair(*it, convert) << "\n";
  }
  sout << "\n";

  sout << "Grand canonical swaps: " << std::endl;
  for (auto it = list.grand_canonical_swap().begin();
       it != list.grand_canonical_swap().end(); ++it) {
    sout << "  " << cand_pair(it->cand_a, convert) << " -> "
         << cand_pair(it->cand_b, convert) << "\n";
  }
  sout << "\n";
  return sout;
}

}  // namespace CASM