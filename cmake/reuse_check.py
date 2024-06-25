#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

import os
import sys
from pathlib import Path
from tempfile import TemporaryDirectory
import subprocess
import shutil

try:
    src = Path(os.environ["I3_SRC"])
except:
    sys.stderr.write("I3_SRC is not set, doing nothing\n")
    sys.exit(-1)

tempdir = TemporaryDirectory()

whitelist_files = [
    "CODEOWNERS",
    "CONTRIBUTING.rst",
    "Brewfile",
    "pyproject.toml",
    ".editorconfig",
    "requirements.txt",
    ".gitignore",
    ".pre-commit-config.yaml",
]

whitelist = [
    ".github",
    ".reuse",
    "astro",
    "cmake",
    "dataclasses",
    "dataio",
    "docs",
    "hdfwriter",
    "icetray",
    "interfaces",
    "LICENSES",
    "phys-services",
    "serialization",
    "steamshovel",
    "tableio",
]

ignore = shutil.ignore_patterns("*.pyc")

for x in whitelist_files:
    shutil.copy(src / x, Path(tempdir.name) / x)

for x in whitelist:
    shutil.copytree(src / x, Path(tempdir.name) / x, ignore=ignore)

ret = subprocess.run(["reuse", "lint"], cwd=tempdir.name, check=False)
print("reuse returned ", ret.returncode)
sys.exit(ret.returncode)
