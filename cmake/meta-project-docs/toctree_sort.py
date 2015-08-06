# Ignore case when sorting toctree
from functools import partial
from sphinx.directives import TocTree
from docutils.parsers.rst import directives

print("Loading toctree_sort extension")

def sort_ignore_case(input,first=None):
    if first:
        input = sort_first(input,first)
    if isinstance(input,basestring):
        return input.lower()
    elif isinstance(input,(list,tuple)):
        return [sort_ignore_case(x,first=first) for x in input]
    else:
        return input

def sort_first(input,first=None,**kwargs):
    if not first or not isinstance(first,(tuple,list)):
        return input
    if isinstance(input,basestring):
        for k in first:
            if k.lower() in input.lower():
                return '_'
    elif isinstance(input,(list,tuple)):
        return [sort_first(x,first=first) for x in input]
    return input

class SortedTocTree(TocTree):
    option_spec = dict(TocTree.option_spec,
                       sorted=directives.flag,
                       ignorecase=directives.flag,
                       first=directives.unchanged_required)
    def run(self):
        rst = super(SortedTocTree, self).run()
        if 'sorted' in self.options:
            first = []
            if 'first' in self.options:
                first = [x.strip() for x in self.options.get('first').split(',')]
            if 'ignorecase' in self.options:
                rst[0][0]['entries'].sort(key=partial(sort_ignore_case,first=first))
            else:
                rst[0][0]['entries'].sort(key=partial(sort_first,first=first))
        return rst

def setup(app):
    app.add_directive('toctree', SortedTocTree)
