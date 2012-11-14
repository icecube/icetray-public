
i3_add_pybindings(interfaces
  module.cxx
  I3GeometryService.cxx
  I3CalibrationService.cxx
  I3DetectorStatusService.cxx
  I3EventService.cxx
  I3MetaService.cxx
  I3OMKey2MBID.cxx
  USE_TOOLS boost python
  USE_PROJECTS interfaces dataclasses
  )
