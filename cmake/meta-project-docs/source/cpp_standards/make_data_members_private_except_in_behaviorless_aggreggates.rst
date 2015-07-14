
Make data members private, except in behaviorless aggregates (C-style structs)
------------------------------------------------------------------------------

Information hiding is the key to good software engineering. Prefer making all data members private; private data is the best means that a class can use to preserve its internal consistency.

Here is an example, a class that stores dates. Consider this implementation::

    // a bad design
    class Date {
    public:
        int day_;
        int month_;
    };

Months can range only from 1 to 12, and days only from 1 to 31 or less, depending on the month; but the exposed data can take any values. The responsiblity to check the consistency of the data is pushed onto the user of this class.

If the data are private, the class itself can provide these checks and users can rely on the fact that Date instances passed to them have a valid state::

    // a better design
    class Date {
    private: // private data
        int day_;
        int month_;
    public:
        // a setter which consistency checks
        void SetDate(int month, int day)
        { /* check that 1 <= month <= 12 and
             1 <= day <= max-day for given month here */
          month_ = month;
          day_ = day;
        }

        // read-only access
        int GetMonth() const { return month_; }
        int GetDay() const { return day_; }
    };

Private data is preferred over protected data. Protected data have all the drawbacks of public data, because derived classes still share the responsiblity in keeping the internal state of the base consistent. Base classes should also encapsulate their state against derived classes.

The exception to this rule are value aggregates, also called "C-style **structs**". They simply keep a bunch of data together and are not meant to maintain any internal consistency. **std::pair** and **boost::tuple** are typical examples.
