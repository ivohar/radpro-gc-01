#!/usr/bin/env python3
"""Convert an "original" displayColors C array into optimized colors array.

Usage:
  python convert_colors.py input_file.c output_file.c

This script looks for a C array named `displayColors` with entries like:
  {mr_get_color(0x1f1f1f),
   mr_get_color(0xe9e9e9),
   mr_get_color(0xeaa75e)},

And emits a new C file with the same entries, but with a 4th color added:
  - transformed[0] = TransformColor(original[0], 2)
  - transformed[1] = original[1]
  - transformed[2] = TransformColor(original[2], 3)
  - transformed[3] = barbie_color(original[0])

"""

import argparse
import re
from typing import List, Optional, Tuple


def barbie_color(color: int) -> int:
    # Simulate the macro: (((color & (0x1f<<16))<<3) | ((color & (0x3f<<8))<<2) | ((color & 0x1f)<<3))
    return (((color & (0x1F << 16)) << 3) |
            ((color & (0x3F << 8)) << 2) |
            ((color & 0x1F) << 3))


def linint(x: int, vals: List[Tuple[int, int]]) -> int:
    for i in range(len(vals) - 1):
        x1, y1 = vals[i]
        x2, y2 = vals[i + 1]
        if x1 <= x <= x2:
            if x1 != x2:
                return int(y1 + (y2 - y1) * (x - x1) / (x2 - x1))
            return int(y1)
    return 0


LogColorArray = [
    (0x00, 0x00),
    (0x20, 0x33),
    (0x40, 0x55),
    (0x60, 0x70),
    (0x80, 0x88),
    (0xA0, 0x9E),
    (0xC0, 0xB3),
    (0xE0, 0xC8),
    (0xFF, 0xFF),
]

thirdDeg_colorArray = [
    (0x00, 0x00),
    (0x20, 0x50),
    (0x40, 0x79),
    (0x60, 0x88),
    (0x80, 0x8C),
    (0xA0, 0x90),
    (0xC0, 0x9E),
    (0xE0, 0xBF),
    (0xFF, 0xFF),
]


def interpolate_color(color: int, mode: int) -> int:
    r = (color >> 16) & 0xFF
    g = (color >> 8) & 0xFF
    b = color & 0xFF

    arr = thirdDeg_colorArray if mode == 3 else LogColorArray

    r = linint(r, arr)
    g = linint(g, arr)
    b = linint(b, arr)
    return (r << 16) | (g << 8) | b


def transform_color(color: int, displayFNIRSI: int) -> int:
    if displayFNIRSI == 1:
        return barbie_color(color)
    if displayFNIRSI in (2, 3):
        return interpolate_color(color, displayFNIRSI)
    return color


def parse_display_colors(lines: List[str]) -> List[Tuple[List[str], Optional[List[int]]]]:
    """Parse a displayColors array from a C file.

    Returns a list of tuples (leading_lines, rgb_values).
      - leading_lines: list of lines that should be emitted verbatim before the entry.
      - rgb_values: [r,g,b] values as integers, or None for directives (#if/#endif) or blank lines.
    """

    in_array = False
    pending_lines: List[str] = []
    entries: List[Tuple[List[str], Optional[List[int]]]] = []

    array_start_re = re.compile(r"static\s+const\s+mr_color_t\s+displayColors\b")
    hex_re = re.compile(r"0x[0-9A-Fa-f]+")

    for line in lines:
        stripped = line.strip()
        if not in_array:
            if array_start_re.search(line):
                in_array = True
                continue
            # ignore everything until we reach the array
            continue

        # We are inside the array
        if array_start_re.search(line):
            # Ignore any repeated declarations inside the array
            continue

        pending_lines.append(line)

        if stripped.startswith("#if") or stripped.startswith("#endif"):
            # Keep preprocessor directives as-is
            entries.append((pending_lines[:], None))
            pending_lines.clear()
            continue

        if stripped.startswith("};"):
            # End of array (might be "};" or "};};")
            if pending_lines:
                # Keep anything before the closing line(s), but drop the closing tokens.
                pending_lines = pending_lines[:-1]
                if pending_lines:
                    entries.append((pending_lines[:], None))
            break

        # Look for the end of an entry ("},")
        if "}," in stripped:
            entry_text = "".join(pending_lines)
            hexes = hex_re.findall(entry_text)
            if len(hexes) >= 3:
                rgb = [int(hexes[i], 16) for i in range(3)]
                entries.append((pending_lines[:], rgb))
            else:
                # Keep whatever this is, even if it doesn't parse
                entries.append((pending_lines[:], None))
            pending_lines.clear()

    return entries


def format_output(entries: List[Tuple[List[str], Optional[List[int]]]]) -> List[str]:
    out: List[str] = []
    out.append("static const mr_color_t displayColors[][4] = {\n")

    for lines, rgb in entries:
        if rgb is None:
            # Either a comment/header line or a preprocessor directive.
            for l in lines:
                if "static const mr_color_t displayColors" in l:
                    continue
                if l.strip().startswith("};"):
                    continue
                out.append(l)
            continue

        # Keep any comment lines/padding before the actual entry
        prefix = []
        entry_lines = []
        for l in lines:
            if "mr_get_color" in l:
                entry_lines.append(l)
            else:
                prefix.append(l)

        out.extend(prefix)

        r, g, b = rgb
        transformed1 = transform_color(r, 2)
        transformed2 = g
        transformed3 = transform_color(b, 3)
        transformed4 = barbie_color(r)

        # Output in the same style as new_colors.c
        out.append(
            f"{{mr_get_color(0x{transformed1:06X}), mr_get_color(0x{transformed2:06X}), mr_get_color(0x{transformed3:06X}), mr_get_color(0x{transformed4:06X})}},\n"
        )

    out.append("};\n")
    return out


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Convert an original displayColors C file to a new displayColors[][][4] file."
    )
    parser.add_argument("input", help="Input C file (original colors)")
    parser.add_argument("output", help="Output C file (new colors)")
    args = parser.parse_args()

    with open(args.input, "r", encoding="utf-8") as f:
        lines = f.readlines()

    entries = parse_display_colors(lines)

    out_lines = format_output(entries)

    with open(args.output, "w", encoding="utf-8") as f:
        f.writelines(out_lines)

    print(f"Wrote {len(out_lines)} lines to {args.output}")


if __name__ == "__main__":
    main()
