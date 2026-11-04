#include "../../installer/media_id_validator.h"
#include <cassert>
#include <iostream>

using namespace LibertyRecomp::Installer;

void TestMediaIdValidation() {
  // Test 1: USA Retail valid
  auto res1 = ValidateGta4Media(0x545407F2, 0x6AC07221);
  assert(res1.status == MediaIdValidationStatus::ValidSupportedUsaTU8);
  assert(res1.status_message.find("Supported USA Retail Disc") != std::string::npos);

  // Test 2: Alternative Media ID 0675959C / 06759S9C
  auto res2 = ValidateGta4Media(0x545407F2, 0x0675959C, "06759S9C");
  assert(res2.status == MediaIdValidationStatus::AlternativeLegitimateMediaIdPendingTU8);
  assert(res2.diagnostic_explanation.find("legitimate alternative Grand Theft Auto IV") != std::string::npos);

  // Test 3: Wrong Title ID
  auto res3 = ValidateGta4Media(0x12345678, 0x6AC07221);
  assert(res3.status == MediaIdValidationStatus::InvalidTitleId);

  // Test 4: Unrecognized Media ID
  auto res4 = ValidateGta4Media(0x545407F2, 0xDEADBEEF);
  assert(res4.status == MediaIdValidationStatus::UnsupportedOrModdedMedia);

  std::cout << "[PASS] Media ID Validation & Alternative Disc Revision Handling" << std::endl;
}

int main() {
  std::cout << "Running Media ID Validator Unit Tests..." << std::endl;
  TestMediaIdValidation();
  std::cout << "All Media ID Validator Unit Tests Passed Successfully!" << std::endl;
  return 0;
}
