#
# $Id$
#


if (PAL_FOUND)

  colormsg(GREEN "  + PAL found")

  i3_project(astro
    PYTHON_DIR python
    DOCS_DIR resources/docs)

  i3_add_library(astro private/astro/*.cxx
    USE_TOOLS boost pal
    USE_PROJECTS serialization icetray dataclasses tableio
    )

  i3_test_scripts(resources/test/*.py)
  set_tests_properties(
    astro::test_inverse.py
    astro::test_radec.py
    PROPERTIES LABELS RUN_LONG
    )
  set_tests_properties(
    astro::test_sun_and_moon.py
    PROPERTIES DEPENDS astro::test_tableio.py
    )

  i3_add_pybindings(astro
      private/pybindings/I3Astro.cxx
      USE_TOOLS python
      USE_PROJECTS astro  tableio
      )

else(PAL_FOUND)
  colormsg(YELLOW "  - PAL not found, skipping build of ${PROJECT_NAME}
       Ask your admin to install PAL. Instructions for PAL can be found at:
       ${CMAKE_SOURCE_DIR}/cmake/resources/docs/tools/pal.rst")
endif(PAL_FOUND)
