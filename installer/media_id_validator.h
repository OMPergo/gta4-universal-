#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <optional>
#include <vector>

namespace LibertyRecomp::Installer {

struct KnownMediaInfo {
  uint32_t media_id = 0;
  std::string_view media_id_hex;
  std::string_view release_region;
  std::string_view description;
  bool is_tu8_compatible = false;
  std::string_view notes;
};

enum class MediaIdValidationStatus {
  ValidSupportedUsaTU8,
  AlternativeLegitimateMediaIdPendingTU8,
  UnsupportedOrModdedMedia,
  InvalidTitleId
};

struct ValidationResult {
  MediaIdValidationStatus status = MediaIdValidationStatus::UnsupportedOrModdedMedia;
  uint32_t title_id = 0;
  uint32_t media_id = 0;
  std::string media_id_string;
  std::string status_message;
  std::string diagnostic_explanation;
};

// Pure analysis function for Xbox 360 GTA IV disc images
ValidationResult ValidateGta4Media(uint32_t title_id, uint32_t media_id, std::string_view raw_media_id_string = "");

std::vector<KnownMediaInfo> GetKnownGta4MediaCatalog();

} // namespace LibertyRecomp::Installer
