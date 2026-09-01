#!/usr/bin/env python3
"""Generate deterministic LVGL A8 icon resources from the asset manifest.

The manifest is JSON-compatible YAML, so this first version has no third-party
Python dependencies. SVG input intentionally supports only `line` and
`polyline` strokes. This keeps the embedded icon pipeline small and
deterministic; richer artwork should be converted with the official LVGL image
converter and then registered through App_UiAssets.
"""

from __future__ import annotations

import argparse
import json
import math
import re
import sys
import xml.etree.ElementTree as ET
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


REPO_ROOT = Path(__file__).resolve().parents[2]
DEFAULT_MANIFEST = REPO_ROOT / "assets" / "manifest.yaml"
GENERATED_DIR = REPO_ROOT / "src" / "lvgl_app" / "assets" / "generated"
HEADER_PATH = GENERATED_DIR / "App_UiIcons.h"
SOURCE_PATH = GENERATED_DIR / "App_UiIcons.c"
REPORT_PATH = GENERATED_DIR / "assets_report.json"
SUPERSAMPLE = 4
IDENTIFIER_RE = re.compile(r"^[a-z][a-z0-9_]*$")


@dataclass(frozen=True)
class Segment:
    x1: float
    y1: float
    x2: float
    y2: float
    stroke_width: float


@dataclass(frozen=True)
class Icon:
    asset_id: str
    source: Path
    symbol: str
    width: int
    height: int
    pixels: bytes


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--manifest",
        type=Path,
        default=DEFAULT_MANIFEST,
        help="JSON-compatible YAML manifest path",
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help="fail if committed generated files differ from expected output",
    )
    return parser.parse_args()


def load_manifest(path: Path) -> dict:
    try:
        manifest = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ValueError(f"manifest not found: {path}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(
            f"{path} must remain JSON-compatible YAML: {exc}"
        ) from exc

    if manifest.get("schema_version") != 1:
        raise ValueError("unsupported or missing manifest schema_version")
    for section in ("icons", "images", "fonts"):
        if not isinstance(manifest.get(section), list):
            raise ValueError(f"manifest section '{section}' must be a list")
    return manifest


def parse_number(value: str | None, field: str) -> float:
    if value is None:
        raise ValueError(f"missing SVG attribute: {field}")
    try:
        return float(value)
    except ValueError as exc:
        raise ValueError(f"invalid SVG number for {field}: {value}") from exc


def parse_points(value: str | None) -> list[tuple[float, float]]:
    if value is None:
        raise ValueError("polyline is missing points")
    tokens = [token for token in re.split(r"[\s,]+", value.strip()) if token]
    if len(tokens) < 4 or len(tokens) % 2 != 0:
        raise ValueError(f"invalid polyline points: {value}")
    numbers = [float(token) for token in tokens]
    return list(zip(numbers[0::2], numbers[1::2]))


def local_name(tag: str) -> str:
    return tag.rsplit("}", 1)[-1]


def parse_svg(path: Path) -> tuple[tuple[float, float, float, float], list[Segment]]:
    if not path.is_file():
        raise ValueError(f"icon source not found: {path}")

    root = ET.parse(path).getroot()
    view_box_values = [
        float(token)
        for token in re.split(r"[\s,]+", root.attrib.get("viewBox", "").strip())
        if token
    ]
    if len(view_box_values) != 4:
        raise ValueError(f"{path}: SVG requires a four-value viewBox")
    min_x, min_y, view_width, view_height = view_box_values
    if view_width <= 0 or view_height <= 0:
        raise ValueError(f"{path}: SVG viewBox dimensions must be positive")

    segments: list[Segment] = []
    for element in root.iter():
        tag = local_name(element.tag)
        if tag not in ("line", "polyline"):
            continue
        stroke_width = parse_number(element.attrib.get("stroke-width"), "stroke-width")
        if stroke_width <= 0:
            raise ValueError(f"{path}: stroke-width must be positive")

        if tag == "line":
            points = [
                (
                    parse_number(element.attrib.get("x1"), "x1"),
                    parse_number(element.attrib.get("y1"), "y1"),
                ),
                (
                    parse_number(element.attrib.get("x2"), "x2"),
                    parse_number(element.attrib.get("y2"), "y2"),
                ),
            ]
        else:
            points = parse_points(element.attrib.get("points"))

        for first, second in zip(points, points[1:]):
            segments.append(
                Segment(first[0], first[1], second[0], second[1], stroke_width)
            )

    if not segments:
        raise ValueError(f"{path}: no supported line/polyline strokes found")
    return (min_x, min_y, view_width, view_height), segments


def distance_to_segment(x: float, y: float, segment: Segment) -> float:
    dx = segment.x2 - segment.x1
    dy = segment.y2 - segment.y1
    length_squared = dx * dx + dy * dy
    if length_squared == 0:
        return math.hypot(x - segment.x1, y - segment.y1)
    projection = ((x - segment.x1) * dx + (y - segment.y1) * dy) / length_squared
    projection = max(0.0, min(1.0, projection))
    nearest_x = segment.x1 + projection * dx
    nearest_y = segment.y1 + projection * dy
    return math.hypot(x - nearest_x, y - nearest_y)


def rasterize_a8(
    view_box: tuple[float, float, float, float],
    segments: Iterable[Segment],
    width: int,
    height: int,
) -> bytes:
    min_x, min_y, view_width, view_height = view_box
    segment_list = list(segments)
    samples_per_pixel = SUPERSAMPLE * SUPERSAMPLE
    pixels = bytearray()

    for pixel_y in range(height):
        for pixel_x in range(width):
            covered = 0
            for sample_y in range(SUPERSAMPLE):
                for sample_x in range(SUPERSAMPLE):
                    x = min_x + (
                        pixel_x + (sample_x + 0.5) / SUPERSAMPLE
                    ) * view_width / width
                    y = min_y + (
                        pixel_y + (sample_y + 0.5) / SUPERSAMPLE
                    ) * view_height / height
                    if any(
                        distance_to_segment(x, y, segment)
                        <= segment.stroke_width / 2.0
                        for segment in segment_list
                    ):
                        covered += 1
            pixels.append(round(255 * covered / samples_per_pixel))
    return bytes(pixels)


def load_icons(manifest: dict) -> list[Icon]:
    icons: list[Icon] = []
    seen_ids: set[str] = set()
    seen_symbols: set[str] = set()

    for entry in manifest["icons"]:
        asset_id = entry.get("id", "")
        symbol = entry.get("symbol", "")
        if not IDENTIFIER_RE.fullmatch(asset_id):
            raise ValueError(f"invalid icon id: {asset_id!r}")
        if not IDENTIFIER_RE.fullmatch(symbol):
            raise ValueError(f"invalid icon symbol: {symbol!r}")
        if asset_id in seen_ids:
            raise ValueError(f"duplicate icon id: {asset_id}")
        if symbol in seen_symbols:
            raise ValueError(f"duplicate icon symbol: {symbol}")
        if entry.get("format") != "A8":
            raise ValueError(f"{asset_id}: only A8 icons are supported")

        width = entry.get("width")
        height = entry.get("height")
        if not isinstance(width, int) or not isinstance(height, int):
            raise ValueError(f"{asset_id}: width and height must be integers")
        if not 1 <= width <= 256 or not 1 <= height <= 256:
            raise ValueError(f"{asset_id}: dimensions must be between 1 and 256")

        source = REPO_ROOT / entry.get("source", "")
        view_box, segments = parse_svg(source)
        icons.append(
            Icon(
                asset_id=asset_id,
                source=source,
                symbol=symbol,
                width=width,
                height=height,
                pixels=rasterize_a8(view_box, segments, width, height),
            )
        )
        seen_ids.add(asset_id)
        seen_symbols.add(symbol)
    return icons


def format_bytes(data: bytes) -> str:
    lines = []
    for offset in range(0, len(data), 12):
        values = ", ".join(f"0x{value:02X}" for value in data[offset : offset + 12])
        lines.append(f"    {values},")
    return "\n".join(lines)


def generate_header(icons: list[Icon]) -> str:
    declarations = "\n".join(
        f"extern const lv_image_dsc_t {icon.symbol};" for icon in icons
    )
    return f"""/* Auto-generated by tools/assets/build_assets.py. Do not edit. */
#ifndef APP_UI_ICONS_GENERATED_H
#define APP_UI_ICONS_GENERATED_H

#include "lvgl.h"

{declarations}

#endif /* APP_UI_ICONS_GENERATED_H */
"""


def generate_source(icons: list[Icon]) -> str:
    blocks = []
    for icon in icons:
        blocks.append(
            f"""static const uint8_t {icon.symbol}_map[] = {{
{format_bytes(icon.pixels)}
}};

const lv_image_dsc_t {icon.symbol} = {{
    .header.magic = LV_IMAGE_HEADER_MAGIC,
    .header.cf = LV_COLOR_FORMAT_A8,
    .header.flags = 0,
    .header.w = {icon.width},
    .header.h = {icon.height},
    .header.stride = {icon.width},
    .data_size = sizeof({icon.symbol}_map),
    .data = {icon.symbol}_map,
}};"""
        )
    joined = "\n\n".join(blocks)
    return f"""/* Auto-generated by tools/assets/build_assets.py. Do not edit. */
#include "App_UiIcons.h"

{joined}
"""


def generate_report(icons: list[Icon]) -> str:
    report = {
        "schema_version": 1,
        "total_bytes": sum(len(icon.pixels) for icon in icons),
        "icons": [
            {
                "id": icon.asset_id,
                "source": icon.source.relative_to(REPO_ROOT).as_posix(),
                "symbol": icon.symbol,
                "format": "A8",
                "width": icon.width,
                "height": icon.height,
                "bytes": len(icon.pixels),
            }
            for icon in icons
        ],
    }
    return json.dumps(report, ensure_ascii=False, indent=2) + "\n"


def sync_file(path: Path, expected: str, check: bool) -> bool:
    current = path.read_text(encoding="utf-8") if path.exists() else None
    if current == expected:
        return True
    if check:
        print(f"out of date: {path.relative_to(REPO_ROOT)}", file=sys.stderr)
        return False
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="\n") as file:
        file.write(expected)
    print(f"generated: {path.relative_to(REPO_ROOT)}")
    return True


def main() -> int:
    args = parse_args()
    try:
        manifest = load_manifest(args.manifest.resolve())
        icons = load_icons(manifest)
    except (OSError, ValueError, ET.ParseError) as exc:
        print(f"asset generation failed: {exc}", file=sys.stderr)
        return 1

    results = [
        sync_file(HEADER_PATH, generate_header(icons), args.check),
        sync_file(SOURCE_PATH, generate_source(icons), args.check),
        sync_file(REPORT_PATH, generate_report(icons), args.check),
    ]
    if not all(results):
        return 1
    if args.check:
        print(f"asset generation is current ({len(icons)} icons)")
    else:
        print(f"asset generation complete ({len(icons)} icons)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
