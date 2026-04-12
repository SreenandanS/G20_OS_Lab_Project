#!/usr/bin/env python3

from pathlib import Path
import sys

from PIL import Image, ImageDraw, ImageFont


def main():
    if len(sys.argv) != 3:
        print("usage: render_text_capture.py <input.txt> <output.png>", file=sys.stderr)
        return 1

    input_path = Path(sys.argv[1])
    output_path = Path(sys.argv[2])
    lines = input_path.read_text().splitlines() or [""]

    try:
        font = ImageFont.truetype("Menlo.ttc", 22)
    except OSError:
        font = ImageFont.load_default()

    max_width = max(font.getbbox(line or " ")[2] for line in lines)
    line_height = font.getbbox("Ag")[3] + 8
    padding = 28
    width = max_width + padding * 2
    height = len(lines) * line_height + padding * 2

    image = Image.new("RGB", (width, height), "#10151c")
    draw = ImageDraw.Draw(image)
    y = padding
    for line in lines:
      draw.text((padding, y), line, fill="#e6edf3", font=font)
      y += line_height

    output_path.parent.mkdir(parents=True, exist_ok=True)
    image.save(output_path)
    print(f"saved {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
