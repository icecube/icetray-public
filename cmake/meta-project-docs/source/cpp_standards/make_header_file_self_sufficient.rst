
Make header files self-sufficient.
----------------------------------

Be sure that the path to your public header files, as seen from other
projects, makes sense.  Don't put #includes of tools in your header
files, otherwise I will have to add those tools to my ``USES_TOOLS``
when I use your project.

