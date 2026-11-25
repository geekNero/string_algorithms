"""Generate clangd commands from the same flags used by Make."""

import json
from pathlib import Path
import sys

args = sys.argv[1:]
separator = args.index("--")
common, test_flags = args[:separator], args[separator + 1:]
entries = []
for source, output, flags in (
    ("src/in_place_fingerprinting.cppm", "build/fingerprint.pcm", ["--precompile"]),
    ("tests/oracle.cppm", "build/oracle.pcm", ["--precompile"]),
    ("tests/test_fingerprint.cpp", "build/test_fingerprint.o", test_flags + ["-c"]),
):
    entries.append({
        "directory": str(Path.cwd()),
        "file": source,
        "output": output,
        "arguments": common + flags + [source, "-o", output],
    })

destination = Path("compile_commands.json")
content = json.dumps(entries, indent=2) + "\n"
if not destination.exists() or destination.read_text() != content:
    temporary = destination.with_suffix(".json.tmp")
    temporary.write_text(content)
    temporary.replace(destination)
