
Don't give away your internals
------------------------------

Avoid returning handles to internal data managed by your class, so clients won't uncontrollably modify state that your object thinks it owns. Also see :doc:`make_data_members_private_except_in_behaviorless_aggreggates`.

An example of what should be avoided::

    class Socket {
    public:
        // ... constructor that opens a handle
        // ... destructor that closes a handle
        int GetHandle() const { return handle_; } // avoid this
    private:
        int handle_; // perhaps an OS resource handle
    };

The handle represents a state in the OS that may be changed by anyone with access to the handle. The class has no way of knowing about external changes, and cannot preserve its internal consistency.
