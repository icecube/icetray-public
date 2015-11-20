
try:
    from matplotlib.sphinxext.plot_directive import setup
except ImportError:
    # construct a placeholder that takes the same arguments as the plot directive
    from docutils.parsers.rst import directives
    from docutils import nodes
    from sphinx.util.compat import make_admonition
    from sphinx.locale import _
    
    class missing_plot(nodes.Admonition, nodes.Element):
        pass
    
    def visit_missing_plot_node(self, node):
        self.visit_admonition(node)

    def depart_missing_plot_node(self, node):
        self.depart_admonition(node)
    
    def plot_directive(name, arguments, options, content, lineno,
                       content_offset, block_text, state, state_machine):
        env = state.document.settings.env

        targetid = "plot-%d" % env.new_serialno('plot')
        targetnode = nodes.target('', '', ids=[targetid])

        ad = make_admonition(missing_plot, name, [_('Install matplotlib to render this plot')], options,
                             content, lineno, content_offset,
                             block_text, state, state_machine)

        return [targetnode] + ad
    
    def _option_boolean(arg):
        if not arg or not arg.strip():
            # no argument given, assume used as a flag
            return True
        elif arg.strip().lower() in ('no', '0', 'false'):
            return False
        elif arg.strip().lower() in ('yes', '1', 'true'):
            return True
        else:
            raise ValueError('"%s" unknown boolean' % arg)


    def _option_context(arg):
        if arg in [None, 'reset']:
            return arg
        else:
            raise ValueError("argument should be None or 'reset'")
        return directives.choice(arg, ('None', 'reset'))


    def _option_format(arg):
        return directives.choice(arg, ('python', 'doctest'))


    def _option_align(arg):
        return directives.choice(arg, ("top", "middle", "bottom", "left", "center",
                                       "right"))
    
    def setup(app):
        setup.app = app
        setup.config = app.config
        setup.confdir = app.confdir

        options = {'alt': directives.unchanged,
                   'height': directives.length_or_unitless,
                   'width': directives.length_or_percentage_or_unitless,
                   'scale': directives.nonnegative_int,
                   'align': _option_align,
                   'class': directives.class_option,
                   'include-source': _option_boolean,
                   'format': _option_format,
                   'context': _option_context,
                   'nofigs': directives.flag,
                   'encoding': directives.encoding
                   }

        app.add_node(missing_plot,
                     html=(visit_missing_plot_node, depart_missing_plot_node),
                     latex=(visit_missing_plot_node, depart_missing_plot_node),
                     text=(visit_missing_plot_node, depart_missing_plot_node))

        app.add_directive('plot', plot_directive, True, (0, 2, False), **options)
        app.add_config_value('plot_pre_code', None, True)
        app.add_config_value('plot_include_source', False, True)
        app.add_config_value('plot_html_show_source_link', True, True)
        app.add_config_value('plot_formats', ['png', 'hires.png', 'pdf'], True)
        app.add_config_value('plot_basedir', None, True)
        app.add_config_value('plot_html_show_formats', True, True)
        app.add_config_value('plot_rcparams', {}, True)
        app.add_config_value('plot_apply_rcparams', False, True)
        app.add_config_value('plot_working_directory', None, True)
        app.add_config_value('plot_template', None, True)

