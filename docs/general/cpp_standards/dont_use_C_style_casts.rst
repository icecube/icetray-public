.. SPDX-FileCopyrightText: 2024 The IceTray Contributors
..
.. SPDX-License-Identifier: BSD-2-Clause

Don't use C-style casts
-----------------------

Age doesn't always imply wisdom. C-style casts have different (and often
dangerous) semantics depending on context, all disguised behind a single
syntax.  Replacing C-style casts with C++-style casts help guard against
unexpected errors.
