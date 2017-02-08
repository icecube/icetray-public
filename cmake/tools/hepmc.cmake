# This definition will fail to find version 3 of the library, due to the removal of HepMCDefs.h. 
# This is more of a feature than a bug, since code compatible with version 2 will generally not
# be compatible with version 3, and so once version 3 is released we will need to give this more 
# thought and care. 

TOOLDEF (hepmc
  include
  HepMC/HepMCDefs.h
  lib
  NONE
  HepMC
  )
