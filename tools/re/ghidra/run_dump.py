#!/usr/bin/env python3
"""Run a GhidraScript (PyGhidra) against a program inside an existing Ghidra project.

Why this exists: Ghidra 12.1.4's `analyzeHeadless` launches a plain JVM, so a `.py` post-script
fails with "Ghidra was not started with PyGhidra. Python is not available". The supported way to
run Python scripts is to start the JVM through PyGhidra. This driver does exactly that: it starts
PyGhidra, opens the given project, opens one program, and executes the script with the program as
its `currentProgram`.

Usage (from the workspace root, with a PyGhidra-enabled python):

  local/venv-pyghidra/bin/python tools/re/ghidra/run_dump.py \
      --project-dir local/ghidra/persistent-20261020 \
      --project-name SotC \
      --program /eboot.elf \
      --script tools/re/ghidra/dump_functions.py \
      --out local/validation/ghidra-decompile-20261101/dossier.txt \
      0x2569d0 0x1bdf30

The driver never saves the program: GhidraScript.execute runs without a save point, and the
session is closed without committing. Pass --check-project to print the project fingerprint
(program count and names) instead of running a script.

Exit codes: 0 success, 1 script error, 2 usage/environment problem.
"""

from __future__ import annotations

import argparse
import os
import sys
from pathlib import Path

DEFAULT_INSTALL = Path("/Users/dehydratedflask/Applications/ghidra_12.1.4_PUBLIC")


def parse_args(argv):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--install-dir", default=os.environ.get("GHIDRA_INSTALL_DIR", str(DEFAULT_INSTALL)),
                        help="Ghidra installation directory (default: %(default)s)")
    parser.add_argument("--project-dir", required=True, help="directory holding the .gpr/.rep")
    parser.add_argument("--project-name", required=True, help="project name without the extension")
    parser.add_argument("--program", default="/eboot.elf", help="project path of the program (default: %(default)s)")
    parser.add_argument("--script", help="GhidraScript to run (.py, .java, ...)")
    parser.add_argument("--out", help="output path passed to the script as its first argument")
    parser.add_argument("--check-project", action="store_true",
                        help="only list the project's programs; do not run a script")
    parser.add_argument("rvas", nargs="*", help="RVAs passed to the script after the output path")
    args = parser.parse_args(argv)
    if not args.check_project and not args.script:
        parser.error("either --script or --check-project is required")
    return args


def main(argv) -> int:
    args = parse_args(argv)

    install_dir = Path(args.install_dir)
    if not (install_dir / "Ghidra" / "application.properties").is_file():
        print(f"ERROR: {install_dir} is not a Ghidra installation", file=sys.stderr)
        return 2
    project_dir = Path(args.project_dir)
    if not project_dir.is_dir():
        print(f"ERROR: project directory {project_dir} is missing", file=sys.stderr)
        return 2

    import pyghidra  # imported after argparse so --help works without a JVM

    pyghidra.start(install_dir=install_dir)

    from ghidra.framework.model import DomainFile  # noqa: F401  (ensures the class path is live)
    from pyghidra import open_project, program_context, ghidra_script

    project = open_project(project_dir, args.project_name)
    try:
        if args.check_project:
            def walk(folder, prefix=""):
                name = f"{prefix}/{folder.getName()}"
                for child in folder.getFiles():
                    print(f"PROGRAM {name}/{child.getName()}")
                for sub in folder.getFolders():
                    walk(sub, name)
            walk(project.getProjectData().getRootFolder(), "")
            print(f"PROJECT_OPENED {project_dir}/{args.project_name}")
            return 0

        script_args = []
        if args.out:
            script_args.append(str(Path(args.out).resolve()))
        script_args.extend(args.rvas)

        with program_context(project, args.program) as program:
            stdout, stderr = ghidra_script(args.script, project, program, script_args,
                                           echo_stdout=True, echo_stderr=True)
        if stderr.strip():
            print("SCRIPT_STDERR_PRESENT", file=sys.stderr)
            return 1
        return 0
    finally:
        project.close()


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
