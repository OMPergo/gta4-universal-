#include "media_id_validator.h"
#include <sstream>
#include <iomanip>

namespace LibertyRecomp::Installer {

constexpr uint32_t kGta4TitleId = 0x545407F2;
constexpr uint32_t kGta4UsaRetailMediaId = 0x6AC07221;

std::vector<KnownMediaInfo> GetKnownGta4MediaCatalog() {
  return {
    {
      kGta4UsaRetailMediaId,
      "6AC07221",
      "USA / NTSC-U",
      "Grand Theft Auto IV (USA Retail 1.00)",
      true,
      "Fully supported baseline with Title Update 8 (0.0.8.5) XEXP delta patches."
    },
    {
      0x0675959C, // Note: "06759S9C" with 'S' recognized as common OCR / hex typo for '5'
      "0675959C",
      "Europe / PAL",
      "Grand Theft Auto IV (European Retail Disc)",
      false,
      "Legitimate European retail release. Requires European Title Update XEXP delta mapping or matching TU8 update."
    },
    {
      0x0B7CD8DC,
      "0B7CD8DC",
      "Japan / NTSC-J",
      "Grand Theft Auto IV (Japanese Retail Disc)",
      false,
      "Legitimate Japanese retail release. Different executable binary offsets."
    }
  };
}

ValidationResult ValidateGta4Media(uint32_t title_id, uint32_t media_id, std::string_view raw_media_id_string) {
  ValidationResult result;
  result.title_id = title_id;
  result.media_id = media_id;

  std::ostringstream ss;
  ss << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << media_id;
  result.media_id_string = ss.str();

  if (title_id != kGta4TitleId) {
    result.status = MediaIdValidationStatus::InvalidTitleId;
    result.status_message = "Provided image does not match Grand Theft Auto IV Title ID (0x545407F2).";
    result.diagnostic_explanation = "The disc or XEX belongs to an entirely different Xbox 360 title.";
    return result;
  }

  if (media_id == kGta4UsaRetailMediaId) {
    result.status = MediaIdValidationStatus::ValidSupportedUsaTU8;
    result.status_message = "Supported USA Retail Disc (Media ID: 6AC07221).";
    result.diagnostic_explanation = "Matches official retail release; Title Update 8 delta patching supported.";
    return result;
  }

  // Check alternative catalog or raw string (such as 06759S9C)
  if (media_id == 0x0675959C || raw_media_id_string.find("06759") != std::string_view::npos) {
    result.status = MediaIdValidationStatus::AlternativeLegitimateMediaIdPendingTU8;
    result.status_message = "Alternative Disc Revision Detected (Media ID: 0675959C / 06759S9C).";
    result.diagnostic_explanation =
        "This is a legitimate alternative Grand Theft Auto IV disc dump (e.g. PAL/European release). "
        "The current recompilation delta pipeline requires the USA Retail base (6AC07221) and USA Title Update 8 (0.0.8.5). "
        "Alternative media IDs cannot be silently loaded without matching localized Title Update XEX delta descriptors.";
    return result;
  }

  for (const auto& entry : GetKnownGta4MediaCatalog()) {
    if (entry.media_id == media_id) {
      result.status = MediaIdValidationStatus::AlternativeLegitimateMediaIdPendingTU8;
      result.status_message = std::string("Alternative Disc Revision: ") + std::string(entry.description);
      result.diagnostic_explanation = std::string(entry.notes);
      return result;
    }
  }

  result.status = MediaIdValidationStatus::UnsupportedOrModdedMedia;
  result.status_message = "Unsupported or unrecognized Media ID.";
  result.diagnostic_explanation =
      "The Media ID does not match any verified retail GTA IV release. Modified or corrupted ISO images cannot be installed.";
  return result;
}

} // namespace LibertyRecomp::Installer
