"""Convert the runtime test's RGB PPM captures to PNG using only stdlib."""
from pathlib import Path
import argparse
import struct
import zlib


def chunk(kind, data):
    return (struct.pack(">I", len(data)) + kind + data +
            struct.pack(">I", zlib.crc32(kind + data) & 0xffffffff))


def render(directory):
    for source in Path(directory).glob("*.ppm"):
        magic, dimensions, maximum, pixels = source.read_bytes().split(b"\n", 3)
        assert magic == b"P6" and maximum == b"255"
        width, height = map(int, dimensions.split())
        assert len(pixels) == width * height * 3
        rows = b"".join(b"\0" + pixels[y * width * 3:(y + 1) * width * 3]
                        for y in range(height))
        png = b"\x89PNG\r\n\x1a\n"
        png += chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 2, 0, 0, 0))
        png += chunk(b"IDAT", zlib.compress(rows)) + chunk(b"IEND", b"")
        source.with_suffix(".png").write_bytes(png)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("build_directory")
    render(parser.parse_args().build_directory)
