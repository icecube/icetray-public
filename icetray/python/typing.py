# SPDX-FileCopyrightText: 2024 The IceTray Contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from typing import Any, Protocol
import numpy as np

# list_indexing_suite::detail::container_from_object provides a conversion from
# numpy buffers to containers of certain integer and floating types, ignoring shape
class NumericBuffer(Protocol):
    def __array__(self) -> "np.ndarray[Any, np.dtype[np.integer|np.floating]]": ...

