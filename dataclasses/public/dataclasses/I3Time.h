#ifndef I3TIME_H_INCLUDED
#define I3TIME_H_INCLUDED

#include <string>
#include <time.h>

#include <icetray/I3FrameObject.h>
#include <dataclasses/Utility.h>

/**
 * @brief A class for dealing with global times.  
 *
 * Has methods for setting the time in either the Daq UTC convention or
 * in the Modified Julian convention 
 * 
 */

static const unsigned i3time_version_ = 0;

namespace I3TimeUtils
{
  /**
   *Returns true if the year is a leap year
   */
  bool leap_year(const int year);

  
  /** 
   *@brief returns the Modified Julian Date of Jan 1 of the specified year
   */
  int32_t mod_julian_day_start_of_year(int year);

  /**
   * @brief return true if a leap second occurs on the last second of the day 
   * Modified Julian Date of mjd
   */
  bool leap_sec_on_mjd(const double mjd);

  /**
   * @brief returns the number of leapseconds between the specified Modified Julian Dates.
   *
   * Including the leap second at the end of mjd1 but not the one at the end of mjd2.
   * returns negitive value if mjd2 > mjd1
   */
  int32_t leap_seconds_range(const int32_t mjd1,const int32_t mjd2);

  /**
   * @brief Returns the number of leapseconds since the start of the year for 
   * specified Modified Julian Date
   */
  int32_t year_to_date_leap_seconds(const int32_t mjd);
  
  /**
   * @brief return return the number of seconds on Modified Julian Date mjd:
   * 86400 for normal days, 86401 for days with leap second
   */
  int32_t seconds_in_day(const double mjd);

  /**
   * @brief Returns the number of tenths of nanoseconds it the specified year.
   *
   * Takes into account both leap years and leap seconds
   */
  int64_t max_DAQ_time(const int year);
  
  /**
   *Converts a time of type double with I3Units of time (your choice) to
   *the number of tenths of nanoseconds (i.e. "daqtime").  This takes
   *into account rounding.
   */
  int64_t ns_to_daqtime( const double time);
}

class I3Time : public I3FrameObject
{
 public:
  // Used by old Muxer technology (PFMuxer in PnF for example)
  static const char* DRIVING_TIME;
  /**
   * @brief enum for the Month
   */
  enum Month
    {
      Jan = 1, 
      Feb = 2, 
      Mar = 3, 
      Apr = 4, 
      May = 5, 
      Jun = 6,
      Jul = 7,
      Aug = 8,
      Sep = 9,
      Oct = 10,
      Nov = 11,
      Dec = 12,
      BadMonth = 999
    };

  /**
   * @brief enum for days of the week
   */
  enum Weekday
    {
      Sunday = 0,
      Monday = 1,
      Tuesday = 2,
      Wednesday = 3,
      Thursday = 4,
      Friday = 5,
      Saturday = 6,
      BadWeekday = 999
    };

  /**
   * @brief default constructor sets the time to year 0, daq UTC time 0.
   * There's no 'nan' for unsigned and long long
   */
  I3Time();

  /**
   * @brief creates the object with the given times as the DAQ time
   */
  I3Time(int32_t year, int64_t daqTime);
  
  /**
   * @brief creates the object with the given double as ModJulianDay
   */
  explicit I3Time(double mjd);

  ~I3Time();
	
  std::ostream& Print(std::ostream&) const override;
	     
  /**
   * @brief Sets the time in the 'daq' style where
   * @param year The year.  Something like 2005.
   * @param daqTime The number of tenths of nanoseconds since the year began
   */  
  void SetDaqTime(int year, int64_t daqTime);
  
  /**
   * @brief Sets the time in the Modified julian convention
   * @param modJulianDay the modified julian day to set
   * @param sec the number of secs after this modified julian day began
   * @param ns the number of ns after this second began.
   */
  void SetModJulianTime(int32_t modJulianDay,
			int32_t sec,
			double ns);
  /**
   * @brief Sets the time in the Modified julian convention as a direct double
   * @param mjd the modified julian day as double to set
   */
  void SetModJulianTimeDouble(double mjd);
  
  /**
   * @brief Sets the time in the UTC calendar date convention
   * @param year The year. Something like 2005.
   * @param month The month. A number in the range of [1, 12].
   * @param day The day. A number between 1 and the maximun number of days in the month
   * @param hour The hour. A number in the range of [0, 23].
   * @param minute The minute. A number in the range of [0, 59].
   * @param sec The second. A number in the range of [0, 59].
   * @param ns the number of ns in the range of [0,1e9) after this second began.
   */
  void SetUTCCalDate(int year,
		     int month,
		     int day,
		     int hour,
		     int minute,
		     int sec,
		     double ns=0.0);
  
  /**
   * @brief Sets the time in the Unix convention
   * @param unixTime time since the Epoch (00:00:00 UTC, January 1, 1970),
   * measured in seconds ignoring leap seconds
   * @param ns the number of ns after this second began.
   */
  void SetUnixTime(time_t unixTime,double ns=0);

  /**
   * @brief Gets the Modified Julian day
   */
  int32_t GetModJulianDay() const;
  
  /**
   * @brief Gets the number of seconds since this Modified Julian day began
   * @note Modified Julian days begin at midnight
   */
  int32_t GetModJulianSec() const;

  /**
   * @brief Gets the number of microseconds since this second began
   */
  int32_t GetModJulianMicroSec() const;
  
  /**
   * @brief Gets the number of nanoseconds since this second began
   */
  double GetModJulianNanoSec() const;

  /**
   * @brief Gets the Modified Julian day as a double including frational days
   * @note This is a suitable to pass to cooridinate services as the time
   */
  double GetModJulianDayDouble() const;

  /**
   * @brief Gets the time in Unix convention
   * @returns The number of seconds since the Epoch (00:00:00 UTC, January 1, 1970)
   */
  time_t GetUnixTime() const;

  /**
   * @brief Gets the year in the UTC convention
   */
  int GetUTCYear() const;

  /**
   * @brief Gets the appropriate time for the DAQ.  tenths of nanoseconds
   * since the start of the UTC year.
   */  
  int64_t GetUTCDaqTime() const;

  /**
   * @brief Gets the appropriate UTC Month
   */
  Month GetUTCMonth() const ;

  /**
   * @brief Gets the appropriate day of the week 
   */
  Weekday GetUTCWeekday() const;

  /**
   * @brief Gets the day of the month.  This is calendar day, so
   * there is no '0'
   */
  int32_t GetUTCDayOfMonth() const;

  /**
   * @brief Gets the number of seconds since this UTC year started
   */
  int32_t GetUTCSec() const;
  
  /**
   * @brief Gets the number of nanoseconds since this current second started.
   */
  double GetUTCNanoSec() const;  

  /**
   * @brief Gets a string representing the time in UTC
   */
  std::string GetUTCString(std::string format="%Y-%m-%d %H:%M:%S UTC")const;

  /** 
   * @brief returns weather this current second is is a leap second
   */
  bool IsLeapSecond() const;
  
  /**
   * equality operator.  
   * @return true if the times are the same
   * @param rhs the I3Time to compare this one to.
   */
  bool operator==(const I3Time& rhs) const;

  /**
   * inequality operator
   * @return false if the times are different
   * @param rhs the I3Time to compare this one to.
   */
  bool operator!=(const I3Time& rhs) const
    {
      if(rhs == *this)
        return false;
      return true;
    }

  /**
   * Dumps a Month to a string for printing
   */
  static std::string MonthToString(Month m);

  /**
   * @brief dumps a Weekday to a string for printing.
   */
  static std::string WeekdayToString(Weekday w);

   /**
   * comparison operator.
   * Compares first the year and then the DAQ time
   * @return true if the lhs should be ordered before the rhs
   */
  bool operator<(const I3Time& rhs) const;

  /**
   * comparison operator.
   * Compares first the year and then the DAQ time
   * @return true if the lhs should be ordered after the rhs
   */
  bool operator>(const I3Time& rhs) const;

  bool operator<=(const I3Time& rhs) const;

  bool operator>=(const I3Time& rhs) const;

  /**
   *Adds a double (please use I3Units of time) to I3Time
   *Takes into account rounding and leap years
   */
  I3Time operator+(const double) const;
  I3Time operator-(const double) const;

 public: 

  static double modjulianday(int year);
  
  static double modjulianday(int year, int64_t daqTime);

  static double julianday(int year);
  
  static double julianday(int year, int64_t daqTime);

  static int32_t yearOf(double modjulianday);

  static int32_t DayOfYear(int64_t daqTime);

  static int32_t DayOfYear(double modjulianday);

 private:

  int32_t year_;
  int64_t daqTime_;

  friend class icecube::serialization::access;

  template <class Archive>
  void serialize(Archive& ar, unsigned version);
};

/**
 * Returns the difference between two I3Times in nanoseconds
 */
double operator-(const I3Time t1,const I3Time t2);

std::ostream& operator<<(std::ostream& oss, const I3Time& d);

I3_POINTER_TYPEDEFS(I3Time);
I3_CLASS_VERSION(I3Time, i3time_version_);

#endif //I3TIME_H_INCLUDED
