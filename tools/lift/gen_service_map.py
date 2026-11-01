#!/usr/bin/env python3
"""Generate docs/service_map.json from the pinned import manifest.

The map is a metadata census only: library names, symbol names, NIDs, types and the manifest's
own resolution status. It contains no game code, addresses or bytes, and is used to plan the
native service layer (PLAT-01-SERVICE-MAP-01).

Usage:
  python3 tools/lift/gen_service_map.py            # write docs/service_map.json
  python3 tools/lift/gen_service_map.py --check    # compare with the committed file
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
MANIFEST = ROOT / "local/re/imports.json"
OUTPUT = ROOT / "docs/service_map.json"

# Library family -> native service area. Ordered by specificity; first match wins.
AREA_RULES: tuple[tuple[str, str], ...] = (
    ("libSceGnmDriver", "gnm"),
    ("libSceVideoOut", "video_out"),
    ("libSceAudioOut", "audio_out"),
    ("libSceAudio", "audio_out"),
    ("libSceNgs2", "audio_ngs2"),
    ("libSceAjm", "audio_ajm"),
    ("libSceAvPlayer", "av_player"),
    ("libScePad", "input"),
    ("libSceMouse", "input"),
    ("libSceKeyboard", "input"),
    ("libSceSaveData", "save"),
    ("libSceCommonDialog", "dialog"),
    ("libSceMsgDialog", "dialog"),
    ("libSceImeDialog", "dialog"),
    ("libSceSysmodule", "sysmodule"),
    ("libSceUserService", "user"),
    ("libSceSystemService", "system"),
    ("libSceAppContent", "app_content"),
    ("libSceContentSearch", "app_content"),
    ("libSceNet", "net"),
    ("libSceSsl", "net"),
    ("libSceHttp", "net"),
    ("libSceNp", "net"),
    ("libSceRtc", "rtc"),
    ("libSceFiber", "fiber"),
    ("libSceUlt", "fiber"),
    ("libSceLibcInternal", "libc"),
    ("libScePosix", "libc"),
    ("libc", "libc"),
    ("libSceFios2", "file_io"),
    ("libSceSystemGesture", "input"),
    ("libSceErrorDialog", "dialog"),
    ("libSceGameCustomDataDialog", "dialog"),
    ("libSceIme", "dialog"),
    ("libScePngEnc", "image"),
    ("libSceScreenShot", "screenshot"),
    ("libSceShareUtility", "share"),
    ("libSceGameLiveStreaming", "share"),
    ("libSceVideoRecording", "share"),
    ("libSceCoredump", "system"),
    ("libScePlayGo", "app_content"),
    ("libSceRazorCpu", "tools"),
    ("libSceRemoteplay", "net"),
    ("libSceVoice", "audio_out"),
    ("libkernel", "kernel"),
)

# Native modules that already touch an area (documentation only; not a coverage claim).
NATIVE_HINTS: dict[str, tuple[str, ...]] = {
    "gnm": ("native/platform/pm4_decoder.*", "native/platform/pm4_state.*",
            "native/platform/pm4_indirect.*"),
    "video_out": (),
    "audio_out": ("native/core/audio_*",),
    "audio_ngs2": ("native/core/audio_*",),
    "input": ("native/core/input_*", "native/core/input_table_551140.*"),
    "save": ("native/core/save_*",),
    "fiber": ("native/core/thread_*", "native/core/tls_*"),
    "kernel": ("native/core/allocator_*", "native/core/critsec_*", "native/core/thread_*"),
    "libc": ("native/core/lookup_1fc80.*",),
    "file_io": ("scripts/psarc.py", "native/core/save_*"),
    "screenshot": ("native/platform/metal_capture.*", "native/core/screenshot_*"),
    "image": ("native/platform/metal_capture.*",),
}


def area_for(library: str) -> str:
    for prefix, area in AREA_RULES:
        if library.startswith(prefix):
            return area
    return "other"


def build() -> dict:
    manifest = json.loads(MANIFEST.read_text())
    imports = sorted(manifest["imports"], key=lambda row: (row["library"], row["name"] or ""))
    libraries = sorted(manifest["libraries"], key=lambda row: row["name"])

    per_library: dict[str, dict] = {}
    for library in libraries:
        per_library[library["name"]] = {
            "version": library.get("version"),
            "area": area_for(library["name"]),
            "imports": 0,
            "symbols": [],
        }

    for row in imports:
        entry = per_library.setdefault(
            row["library"], {"version": None, "area": area_for(row["library"]), "imports": 0,
                             "symbols": []}
        )
        entry["imports"] += 1
        entry["symbols"].append(
            {
                "name": row.get("name"),
                "nid": row.get("nid"),
                "type": row.get("type"),
                "module": row.get("module"),
                "status": row.get("status"),
            }
        )

    for entry in per_library.values():
        entry["symbols"].sort(key=lambda symbol: (symbol["name"] or "", symbol["nid"] or ""))

    areas: dict[str, dict] = {}
    for name, entry in per_library.items():
        area = areas.setdefault(entry["area"], {"libraries": [], "imports": 0, "native_hints": []})
        area["libraries"].append(name)
        area["imports"] += entry["imports"]
    for area, data in areas.items():
        data["libraries"].sort()
        data["native_hints"] = list(NATIVE_HINTS.get(area, ()))

    return {
        "schema": "sotc-service-map/1",
        "source": {
            "elf_sha256": manifest.get("elf_sha256"),
            "symbol_count": manifest.get("symbol_count"),
            "import_status_counts": manifest.get("import_status_counts"),
        },
        "totals": {
            "libraries": len(per_library),
            "imports": len(imports),
            "areas": len(areas),
        },
        "areas": dict(sorted(areas.items())),
        "libraries": dict(sorted(per_library.items())),
    }


def render(document: dict) -> str:
    return json.dumps(document, indent=2, sort_keys=True) + "\n"


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--check", action="store_true", help="compare with the committed map")
    args = parser.parse_args()

    text = render(build())
    if args.check:
        if not OUTPUT.is_file():
            print(f"FAIL missing {OUTPUT}")
            return 1
        if OUTPUT.read_text() != text:
            print(f"FAIL {OUTPUT.name} is stale; rerun tools/lift/gen_service_map.py")
            return 1
        document = json.loads(text)
        print(
            "OK check_service_map: %d libraries, %d imports, %d areas match the pinned manifest"
            % (document["totals"]["libraries"], document["totals"]["imports"],
               document["totals"]["areas"])
        )
        return 0

    OUTPUT.write_text(text)
    document = json.loads(text)
    print(
        "wrote %s: %d libraries, %d imports, %d areas"
        % (OUTPUT.relative_to(ROOT), document["totals"]["libraries"],
           document["totals"]["imports"], document["totals"]["areas"])
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
