#
# $Id$
#

i3_project(tableio
	DOCS_DIR resources/docs
	PYTHON_DIR python
)

i3_add_library(tableio
  private/tableio/*.cxx
  private/tableio/converter/*.cxx 
  private/tableio/broadcaster/*.cxx
  private/tableio/textwriter/*.cxx
  private/tableio/utils/*.cxx
  private/tableio/detail/*.cxx
  USE_TOOLS boost
  USE_PROJECTS serialization icetray dataclasses
)

i3_add_pybindings(tableio
  private/pybindings/I3TableRowDescription.cxx
  private/pybindings/I3TableRow.cxx
  private/pybindings/I3Converter.cxx
  private/pybindings/I3ConverterBundle.cxx
  private/pybindings/I3TableService.cxx
  private/pybindings/I3Table.cxx
  private/pybindings/I3TableWriter.cxx
  private/pybindings/I3TableTranscriber.cxx
  private/pybindings/I3BroadcastTableService.cxx
  private/pybindings/I3CSVTableService.cxx
  private/pybindings/I3Datatype.cxx
  private/pybindings/type_helpers.cxx
  private/pybindings/dataclasses_converters.cxx
  private/pybindings/module.cxx
  USE_TOOLS boost python
  USE_PROJECTS icetray tableio
  )

i3_test_executable(test 
    private/test/main.cxx
    private/test/I3DOMLaunchConverterTest.cxx
    private/test/I3DatatypeTest.cxx
    private/test/I3DoubleConverterTest.cxx
    private/test/I3TableRowDescriptionTest.cxx
    private/test/I3TableRowTest.cxx
    private/test/I3VectorI3ParticleConverterTest.cxx
    private/test/InheritanceConversionTest.cxx
    USE_PROJECTS dataclasses icetray phys-services tableio)

i3_test_scripts(
    resources/test/test_padding.py
    resources/test/test_pybindings.py
    resources/test/test_recopulsemask.py
    resources/test/test_recopulse_options.py
    resources/test/test_subevents.py
    resources/test/test_I3MCTree.py
    resources/test/test_inheritance_conversion.py
    resources/test/test_converters.py
)

## mark the following tests as skipped, instead of a false "passed"
## see the indvidual tests for the skip logic
set_tests_properties(
    tableio::test_padding.py tableio::test_subevents.py tableio::test_I3MCTree.py
    tableio::test_inheritance_conversion.py
    PROPERTIES SKIP_REGULAR_EXPRESSION " \.\.\. skipped ")
