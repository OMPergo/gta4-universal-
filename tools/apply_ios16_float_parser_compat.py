#!/usr/bin/env python3
"""Apply the iOS 16-compatible locale-independent ReXGlue float parser.

The pinned ReXGlue version uses floating-point std::from_chars overloads
that Apple's SDK marks unavailable before iOS 26. Integer parsing remains
on std::from_chars; only decimal float/double parsing is redirected through
the classic C++ locale stream parser.
"""
from pathlib import Path
import subprocess
import sys

THEFT4_PIN = "88115fefad59cfcb62d17c0c9548a7a900c01a23"
RELATIVE_FILE = Path("glue/rexglue-sdk-main/include/rex/string/numeric.h")


def replace_exact(source: str, old: str, new: str, label: str) -> str:
    if new in source:
        return source
    count = source.count(old)
    if count != 1:
        raise SystemExit(f"{label}: expected one patch anchor, found {count}")
    return source.replace(old, new, 1)


def main() -> None:
    if len(sys.argv) != 2:
        raise SystemExit("usage: apply_ios16_float_parser_compat.py <runtime/theft4>")
    root = Path(sys.argv[1]).resolve()
    actual = subprocess.check_output(
        ["git", "-C", str(root), "rev-parse", "HEAD"], text=True
    ).strip()
    if actual != THEFT4_PIN:
        raise SystemExit(f"Refusing unexpected Theft4 source: {actual} (expected {THEFT4_PIN})")

    path = root / RELATIVE_FILE
    source = path.read_text(encoding="utf-8")
    original = source

    source = replace_exact(
        source,
        "#include <string_view>\n",
        "#include <string_view>\n#include <locale>\n#include <sstream>\n",
        "standard-library includes",
    )
    source = replace_exact(
        source,
        """namespace detail {\n\ntemplate <typename T, typename V = std::make_signed_t<T>>""",
        """namespace detail {\n\n// Preserve from_chars-style prefix consumption while avoiding the Apple SDK\n// availability annotation on floating-point std::from_chars overloads.\ntemplate <typename T>\ninline std::from_chars_result parse_float_compat(const char* first, const char* last, T& value) {\n  std::istringstream stream(std::string(first, last));\n  stream.imbue(std::locale::classic());\n  stream >> std::noskipws >> value;\n  if (stream.fail()) {\n    return {first, std::errc::invalid_argument};\n  }\n  const std::streamoff consumed = static_cast<std::streamoff>(stream.tellg());\n  if (consumed < 0) {\n    return {last, std::errc{}};\n  }\n  return {first + consumed, std::errc{}};\n}\n\ntemplate <typename T, typename V = std::make_signed_t<T>>""",
        "floating parser helper",
    )
    source = replace_exact(
        source,
        """auto [p, error] = std::from_chars(range.data(), range.data() + range.size(), result,\n                                      std::chars_format::general);\n    // TODO(gibbed): do something more with errors?\n    if (error != std::errc()) {""",
        """auto parsed = parse_float_compat(range.data(), range.data() + range.size(), result);\n    // TODO(gibbed): do something more with errors?\n    if (parsed.ec != std::errc()) {""",
        "float/double parser call",
    )
    source = replace_exact(
        source,
        """auto result = std::from_chars(p, end, v.f32[i], std::chars_format::general);""",
        """auto result = detail::parse_float_compat(p, end, v.f32[i]);""",
        "vec128 float parser call",
    )

    if source == original:
        print("iOS 16 float parser compatibility already applied.")
        return
    path.write_text(source, encoding="utf-8")
    print("Applied iOS 16-compatible float/double parsing to pinned ReXGlue source.")


if __name__ == "__main__":
    main()
