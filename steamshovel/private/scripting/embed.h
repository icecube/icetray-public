#ifndef I3_SHOVEL_EMBED_PY_H
#define I3_SHOVEL_EMBED_PY_H

#include <termios.h>
#include <string>

class QWidget;

namespace scripting{

    // uses RAII to set up a context
    class PyInterpreter {
    public:
        PyInterpreter( char* progname );
        ~PyInterpreter();
    };

    // uses RAII to set up a context
    class PyConsole {
    public:
        enum Type {
            None = 0,
            Vanilla,
            IPython_shell,
            IPython_widget,
            Macapp_widget_or_nothing
        };

    private:
        Type type_;
        termios stored_termios_state;

        bool init_ipython_widget();
        bool init_python_shell( bool ipython );

    public:
        // shell context depends on interpreter context
        PyConsole( const PyInterpreter& context,
                       Type requested );
        ~PyConsole();

        Type type() const { return type_; }

        bool embedInto( QWidget* parent ) const;

        void executeScript( const std::string& );
        void executeString( const std::string& );
    };

} // namespace scripting

#endif /* I3_SHOVEL_EMBED_PY_H */
