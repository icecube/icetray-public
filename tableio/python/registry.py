# Copyright  (C) 2010 The Icecube Collaboration
# SPDX-License-Identifier: BSD-2-Clause
#
# $Id: registry.py 107665 2013-06-29 14:39:18Z nwhitehorn $
#
# @version $Revision: 107665 $
# @date $LastChangedDate: 2013-06-29 10:39:18 -0400 (Sat, 29 Jun 2013) $
# @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy: nwhitehorn $
#

from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from icecube.icetray import I3FrameObject
    from icecube._tableio import I3Converter

class I3ConverterRegistry:
    """A mapping of object type->I3Converter.

    This should be considered an implementation detail and subject to change without notice."""
    registry: "dict[type[I3FrameObject],list[type[I3Converter]]]" = dict()
    defaults: "dict[type[I3FrameObject],type[I3Converter]]" = dict()
    @classmethod
    def register(cls,converter: "type[I3Converter]",object_type: "None|type[I3FrameObject]"=None,is_default=False):
        if object_type is None:
            object_type = converter.booked  # type: ignore[attr-defined]
        if object_type not in cls.registry:
            cls.registry[object_type] = []
        converters = cls.registry[object_type]
        converters.append(converter)
        if is_default:
            if object_type in cls.defaults:
                 raise TypeError("Attempted to register '%s' as the default converter for '%s', but '%s' has already been registered as default!" % (converter, object_type, cls.defaults[object_type]))
            cls.defaults[object_type] = converter
    @classmethod
    def update(cls,registry_dict):
        for conv,targets in registry_dict.items():
            if conv in cls.registry:
                cls.registry[conv] += targets
            else:
                cls.registry[conv] = targets
