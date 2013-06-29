
import sys, os, glob, shutil, hashlib, imp, warnings
import re
try:
    from hashlib import md5
except ImportError:
    from md5 import md5
from docutils.parsers.rst import directives
from docutils.parsers.rst import Directive
try:
    # docutils 0.4
    from docutils.parsers.rst.directives.images import align
except ImportError:
    # docutils 0.5
    from docutils.parsers.rst.directives.images import Image
    align = Image.align
from docutils import nodes
import sphinx

options = { }

from pprint import pprint
class ScriptDirective(Directive):

    required_arguments = 1
    optional_arguments = 0
    final_argument_whitespace = True
    option_spec = { }
    has_content = False
    
    def run(self):
        #print "blocktext:", self.block_text
        #print "options:", self.options
        #print "args:", self.arguments
        #print "attributes:", self.state_machine.document.attributes
        thisfile = self.state_machine.document.attributes['source']
        #print "dir:", dir(self)
        #print "setup:", setup.config, dir(setup.config)
        #print setup.app.builder, "\n"
        #pprint(setup.app.srcdir)

        rp = os.path.relpath(thisfile, setup.app.srcdir)
        (h,t) = os.path.split(rp)
        srcscript = os.path.join(setup.app.srcdir, h, self.arguments[0])
        copyto_dir = os.path.join(setup.app.outdir, h)
        
        if (not os.path.isdir(copyto_dir)):
            os.makedirs(copyto_dir)

        shutil.copyfile(srcscript, os.path.join(copyto_dir, self.arguments[0]))

        #self.state_machine.insert_input(["HEE", "HEE"], self.state_machine.input_lines.source(0))

        reference = directives.uri(self.arguments[0])
        self.options['refuri'] = reference
        refnode = nodes.reference(text=self.arguments[0],
                                  **self.options)
        iline = nodes.inline()

        iline += [refnode]

        return [iline]


def setup(app):
    setup.app = app
    setup.config = app.config
    setup.confdir = app.confdir

    app.add_directive('script', ScriptDirective)

