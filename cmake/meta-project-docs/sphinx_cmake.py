print "Loading cmake sphinx extension"

def setup(app):
    app.add_description_unit('cmake', 'cmake', None)   

import warnings

warnings.filterwarnings('ignore', '.*arg is not a Python function.*')
