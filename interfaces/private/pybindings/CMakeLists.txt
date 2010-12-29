
i3_add_pybindings(interfaces
  module.cxx
  I3GeometryService.cxx
  I3CalibrationService.cxx
  I3DetectorStatusService.cxx
  I3EventService.cxx
  I3MetaService.cxx
  USE_TOOLS boost log4cplus python
  USE_PROJECTS interfaces dataclasses
  )
