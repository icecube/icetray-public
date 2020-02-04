#include <string>
#include <dataclasses/jday.h>

#include <icetray/serialization.h>
#include <dataclasses/I3Time.h>
#include <icetray/I3Units.h>
#include <boost/algorithm/string.hpp>


namespace {
/** 
 * @brief This literal contians a list of Modified Julian Date  of days that @a end on a 
 * leap second. Used by I3Time to determine whe leap seconds occur.
 * 
 * @note This contains the mjd of days which @a end on a leap year. 
 * Most communications describing leap seconds give the day which @a starts with 
 * the leap second. Be very careful addind items to this list.
 */
const int32_t leap_sec_list_[] = {41316, 41498, 41682, 42047, 42412, 42777, 43143,
				  43508, 43873, 44238, 44785, 45150, 45515, 46246, 
				  47160, 47891, 48256, 48803, 49168, 49533, 50082, 
				  50629, 51178, 53735, 54831, 56108, 57203, 57753};

/**
 * @brief This vector contians the same information as leap_sec_list but in a std::vector 
 * for easier access. Used by I3Time to determine whe leap seconds occur. 
 * 
 * Cannot declare array literal for std::vector yet.
 */
std::vector<int32_t> leap_sec_list(leap_sec_list_,leap_sec_list_+sizeof(leap_sec_list_)/sizeof(int32_t));

/** 
 * @brief number of seconds in a standard day add 1 for days with a leap second
 */
const int32_t SECONDS_IN_DAY = 86400;
}

int32_t I3TimeUtils::mod_julian_day_start_of_year(int year)
{
  UTinstant i;
  i.year = year;
  i.month = 1;
  i.day = 1;
  i.i_hour = 0;
  i.i_minute = 0;
  i.second = 0;
  return int32_t(JulDate(&i) - 2400000);
}

bool I3TimeUtils::leap_sec_on_mjd(const double mjd){
  return find(leap_sec_list.begin(),leap_sec_list.end(), (const int32_t) mjd)!=leap_sec_list.end();
}

int32_t I3TimeUtils::seconds_in_day(const double mjd){
  return SECONDS_IN_DAY+leap_sec_on_mjd(mjd);
}

int32_t I3TimeUtils::leap_seconds_range(const int32_t mjd1,const int32_t mjd2){
  std::vector<int32_t>::iterator low,up;
  low=lower_bound (leap_sec_list.begin(), leap_sec_list.end(), mjd1);
  up= lower_bound (leap_sec_list.begin(), leap_sec_list.end(), mjd2);
  return int32_t(up-low);
}

int32_t I3TimeUtils::year_to_date_leap_seconds(int32_t mjd)
{
  double julianDay = mjd + 2400000.5;
  UTinstant j;
  j.j_date = julianDay;
  CalDate(&j);
  return I3TimeUtils::leap_seconds_range(mod_julian_day_start_of_year(j.year),mjd);
}

bool I3TimeUtils::leap_year(const int year){
  return ((!(year%4)&&(year%100)) || //divisible by four and not a century year
	   (!(year%400)&&!(year%100))); //a century year and divisible by four hundred
}

int64_t I3TimeUtils::max_DAQ_time(const int year){

  int32_t leap_seconds_in_year = leap_seconds_range(mod_julian_day_start_of_year(year),
						    mod_julian_day_start_of_year(year+1));

  return ((365+I3TimeUtils::leap_year(year))*86400 + leap_seconds_in_year)*10000000000LL;
}

int64_t I3TimeUtils::ns_to_daqtime( const double time){
  //the addition of 0.5 effectively rounds
  return static_cast<int64_t>((10.*time/I3Units::ns)+ 0.5); 
}

// Used by old Muxer technology (PFMuxer in PnF for example)
const char* I3Time::DRIVING_TIME = "DrivingTime";

I3Time::I3Time()
{
  year_ = 0;
  daqTime_ = 0;
}

I3Time::~I3Time() {}

I3Time::I3Time(int32_t year, int64_t daqTime) : year_(year),daqTime_(daqTime) {}

I3Time::I3Time(double mjd) {
  SetModJulianTimeDouble(mjd);
}

void I3Time::SetDaqTime(int year, 
                        int64_t daqTime)
{
  year_ = year;
  daqTime_ = daqTime;
}

void I3Time::SetModJulianTime(int32_t modJulianDay,
                              int32_t sec,
                              double ns)
{
  if (sec <0 ||  sec >= I3TimeUtils::seconds_in_day(modJulianDay))
    {
      log_error("Invalid second: %i!", sec);
      return;
    }
  if (ns <0 ||  ns >= I3Units::second )
    {
      log_error("Invalid nano second: %g!", ns);
      return;
    }

  year_ = yearOf(modJulianDay);
  int32_t daysafteryear = modJulianDay - (int32_t)modjulianday(year_);
  int32_t secsafteryear = daysafteryear * 3600 * 24 + sec;

  daqTime_ =
    (((int64_t)secsafteryear +  I3TimeUtils::year_to_date_leap_seconds(modJulianDay) ) * ((int64_t)(1e10)))
    + ((int64_t)(ns * 10.));
}

void I3Time::SetModJulianTimeDouble(double mjd) {
  if (std::isnan(mjd)) {
    log_error("Calling with NAN not possible; will do nothing");
    return;
  }
  int32_t modJulianDay = (int32_t)mjd; //number of full days
  year_ = yearOf(modJulianDay); // this is the year
  int32_t daysafteryear = modJulianDay - (int32_t)modjulianday(year_); // the full MJDs since Jan 1st that year
  int64_t ticksafteryear = (int64_t)(daysafteryear*3600 * 24 * ((int64_t)(1e10))); // corresponding to that many daq_ticks
  int64_t ticksleap = I3TimeUtils::year_to_date_leap_seconds(modJulianDay)* ((int64_t)(1e10)); // leap-seconds since Jan 1st that year in daq_ticks
  int64_t tickssurplus = (int64_t)((mjd-modJulianDay)*3600*24*((int64_t)(1e10))); // plus daq_ticks held in the decimal tail of the double
  daqTime_ = (int64_t)(ticksafteryear + tickssurplus + ticksleap); // is in sum the daq_ticks
}

void I3Time::SetUTCCalDate(int year, int month, int day, int hour, int minute, int sec, double ns)
{
    int daysOfMonth[12] = {31,28,31,30,31,30,31,31,30,31,30,31}; 
    if(I3TimeUtils::leap_year(year)) daysOfMonth[1] = 29;
    
    if(month<1 || month>12)
    {
        log_error("Invalid month: %i!", month);
        return;
    }

    if(day<1 || day>daysOfMonth[month-1])
    {
        log_error("Invalid day: %i!", day);
        return;
    }
    
    UTinstant i;
    i.year     = year;
    i.month    = month;
    i.day      = day;
    i.i_hour   = 0;
    i.i_minute = 0;
    i.second   = 0;
    
    int32_t modJulDay = (int32_t)(JulDate(&i) - 2400000.5);

    if(hour<0 || hour>23)
    {
        log_error("Invalid hour: %i!", hour);
        return;
    }

    if(minute<0 || minute>59)
    {
        log_error("Invalid minute: %i!", minute);
        return;
    }

    if(sec<0 || sec > ((I3TimeUtils::leap_sec_on_mjd(modJulDay) && hour==23 && minute == 59)?60:59))
    {
        log_error("Invalid second: %i!", sec);
        return;
    }
    
    if(ns<0 || ns>=1e9)
    {
        log_error("Invalid nanosecond: %g!", ns);
        return;
    }
    
    int32_t second = (int32_t)(hour*3600 + minute*60 + sec);
    
    SetModJulianTime(modJulDay, second, ns);
}

void I3Time::SetUnixTime(time_t unixTime,double ns)
{
  if(unixTime < 0) log_fatal("invalid Unix time");
  int32_t mjd = unixTime / 86400 + 40587;
  SetModJulianTime(mjd, unixTime % 86400, ns);
}

time_t I3Time::GetUnixTime() const
{
  int32_t mjd = GetModJulianDay();
  time_t unixtime = (mjd-40587)*86400+GetModJulianSec();
  if (IsLeapSecond()){
    unixtime--;
  }
  return unixtime;  
}

int I3Time::GetUTCYear() const
{
  return year_;
}

int64_t I3Time::GetUTCDaqTime() const
{
  return daqTime_;
}

int32_t I3Time::GetModJulianDay() const
{
  return (int32_t)modjulianday(year_,daqTime_);
}

int32_t I3Time::GetModJulianSec() const
{

  const int32_t mjd = (const int32_t) modjulianday(year_,daqTime_);
  const int32_t daysafteryear = 
    (int32_t)(mjd - modjulianday(year_));
  int32_t secsafteryear = 
    (daqTime_ - daqTime_%((int64_t)(1e10)))/((int64_t)1e10);
  secsafteryear -= I3TimeUtils::year_to_date_leap_seconds(mjd);
  return secsafteryear - daysafteryear * 3600 * 24 ;
}

int32_t I3Time::GetModJulianMicroSec() const
{
  int64_t daqtenthsns = daqTime_ % 10000000000L;
  return (int32_t)(daqtenthsns/10000);
}

double I3Time::GetModJulianNanoSec() const
{
  int64_t daqtenthsns = daqTime_ %((int64_t)1e10);
  return 0.1 * daqtenthsns;
}

double I3Time::GetModJulianDayDouble() const
{
  int32_t mjd = GetModJulianDay();
  return mjd + (GetModJulianSec()-IsLeapSecond()+GetModJulianNanoSec()/1e9)/SECONDS_IN_DAY;
}

I3Time::Month I3Time::GetUTCMonth() const
{
  double julday = julianday(year_,daqTime_); 
  UTinstant i;
  i.j_date = julday;
  CalDate(&i);
  switch(i.month)
    {
    case 1:
      return Jan;
    case 2:
      return Feb;
    case 3:
      return Mar;
    case 4:
      return Apr;
    case 5:
      return May;
    case 6:
      return Jun;
    case 7:
      return Jul;
    case 8:
      return Aug;
    case 9:
      return Sep;
    case 10:
      return Oct;
    case 11:
      return Nov;
    case 12:
      return Dec;
    default:
      return BadMonth;
    }
}

I3Time::Weekday I3Time::GetUTCWeekday() const
{
  double julday = julianday(year_,daqTime_);
  UTinstant i;
  i.j_date = julday;
  CalDate(&i);
  switch(i.weekday)
    {
    case 0:
      return Sunday;
    case 1:
      return Monday;
    case 2:
      return Tuesday;
    case 3:
      return Wednesday;
    case 4:
      return Thursday;
    case 5:
      return Friday;
    case 6:
      return Saturday;
    default:
      return BadWeekday;
    }

}

int32_t I3Time::GetUTCDayOfMonth() const
{
  double julday = julianday(year_,daqTime_);
  UTinstant i;
  i.j_date = julday;
  CalDate(&i);
  return i.day;
}

int32_t I3Time::GetUTCSec() const
{
  int64_t tenthsOfNs = daqTime_ %((int64_t)1e10);
  int32_t daqSecs = (daqTime_ - tenthsOfNs)/((int64_t)1e10);
  return daqSecs;
}

double I3Time::GetUTCNanoSec() const
{
  int64_t daqtenthsns = daqTime_ %((int64_t)1e10);
  return 0.1 * daqtenthsns;
}

std::string I3Time::GetUTCString(std::string format)const
{
  time_t t=GetUnixTime();
  if( IsLeapSecond())
    {
      t-=1;
      boost::replace_all(format, "%S", "60");
    }
  struct tm *tm=gmtime(&t);
  char datestring[256];
  strftime (datestring, sizeof(datestring), format.c_str(), tm);
  return datestring;
}

bool I3Time::IsLeapSecond() const
{
  return GetModJulianSec()==86400;
}

bool I3Time::operator<(const I3Time& rhs) const
{
  if(year_ < rhs.year_)
    return true;
  if(year_ > rhs.year_)
    return false;
  if(daqTime_ < rhs.daqTime_)
    return true;
  return false;
}

bool I3Time::operator>(const I3Time& rhs) const
{
  if(year_ > rhs.year_)
    return true;
  if(year_ < rhs.year_)
    return false;
  if(daqTime_ > rhs.daqTime_)
    return true;
  return false;
}

bool I3Time::operator==(const I3Time& rhs) const
{
  if(year_ == rhs.year_ && daqTime_ == rhs.daqTime_)
    return true;
  return false;
}

bool I3Time::operator<=(const I3Time& rhs) const
{
  if(*this < rhs || *this == rhs)
    return true;
  return false;
}

bool I3Time::operator>=(const I3Time& rhs) const
{
  if(*this > rhs || *this == rhs)
    return true;
  return false;
}

I3Time I3Time::operator+(const double second_term) const
{
  long double daqTime = daqTime_ + (long double)(second_term)*10;
  int32_t year = year_ ;
   
  while (daqTime >= I3TimeUtils::max_DAQ_time(year))
    {
      daqTime -= I3TimeUtils::max_DAQ_time(year);
      year++;
    }

  while (daqTime < 0 )
    {
      year--;      
      daqTime += I3TimeUtils::max_DAQ_time(year);
    }

  return I3Time(year,daqTime+0.5);
}


I3Time I3Time::operator-(const double second_term) const
{
  return *this+(-second_term);
}

double operator-(const I3Time t1,const I3Time t2) 
{

  int32_t mjd1=t1.GetModJulianDay();
  int32_t mjd2=t2.GetModJulianDay();
  int32_t leapseconds =  I3TimeUtils::leap_seconds_range(mjd1,mjd2);
  return 
    ( t1.GetModJulianNanoSec() - t2.GetModJulianNanoSec() ) * I3Units::nanosecond +
    ( t1.GetModJulianSec()     - t2.GetModJulianSec() -leapseconds ) * I3Units::second +
    ( mjd1 - mjd2 ) * I3Units::day;
}

std::ostream& I3Time::Print(std::ostream& oss) const{
  int64_t daqt=GetUTCDaqTime();
  oss << GetUTCString("%Y-%m-%d %H:%M:%S.");
  oss << std::setw(3) << std::setfill('0') << (daqt/10000000)%1000  << ',';
  oss << std::setw(3) << std::setfill('0') << (daqt/10000)%1000 << ',';
  oss << std::setw(3) << std::setfill('0') << (daqt/10)%1000 << ',';
  oss << daqt%10 << " UTC";

  return oss;
}

std::ostream& operator<<(std::ostream& oss, const I3Time& t){
  return(t.Print(oss));
}

std::string I3Time::MonthToString(Month m)
{
  switch(m)
    {
    case Jan:
      return "Jan";
    case Feb:
      return "Feb";
    case Mar:
      return "Mar";
    case Apr:
      return "Apr";
    case May:
      return "May";
    case Jun:
      return "Jun";
    case Jul:
      return "Jul";
    case Aug:
      return "Aug";
    case Sep:
      return "Sep";
    case Oct:
      return "Oct";
    case Nov:
      return "Nov";
    case Dec:
      return "Dec";
    default:
      return "BadMonth";
    }
}

std::string I3Time::WeekdayToString(Weekday w)
{
  switch(w)
    {
    case Sunday:
      return "Sunday";
    case Monday:
      return "Monday";
    case Tuesday:
      return "Tuesday";
    case Wednesday:
      return "Wednesday";
    case Thursday:
      return "Thursday";
    case Friday:
      return "Friday";
    case Saturday:
      return "Saturday";
    default:
      return "BadWeekday";
    }
}

double I3Time::modjulianday(int year)
{
  UTinstant i;
  i.year = year;
  i.month = 1;
  i.day = 1;
  i.i_hour = 0;
  i.i_minute = 0;
  i.second = 0;
  return JulDate(&i) - 2400000.5;
}

double I3Time::modjulianday(int year, int64_t daqTime)
{

  //first calculate mjd without worrying about leap seconds
  //there is probabally a better way to do this but this works just fine
  int64_t tenthsOfNs = daqTime %((int64_t)1e10);
  int64_t daqSecs = (daqTime - tenthsOfNs)/((int64_t)1e10);
  double daqDaysSinceYear = ((double)(daqSecs))/(3600. * 24.);
  double modjulian_of_year = modjulianday(year);
  double mjd = modjulian_of_year + daqDaysSinceYear;

  //now that we know mjd redo with correct seconds
  //don't use year to date leapseconds because we might have gone over the year boundry 
  daqSecs -= I3TimeUtils::leap_seconds_range(I3TimeUtils::mod_julian_day_start_of_year(year),
					     mjd);
  daqDaysSinceYear = ((double)(daqSecs))/(3600. * 24.);
  modjulian_of_year = modjulianday(year);

  return modjulian_of_year + daqDaysSinceYear;   
}

double I3Time::julianday(int year)
{
  UTinstant i;
  i.year = year;
  i.month = 1;
  i.day = 1;
  i.i_hour = 0;
  i.i_minute = 0;
  i.second = 0;
  return JulDate(&i);
}
  
double I3Time::julianday(int year, int64_t daqTime)
{
  return modjulianday(year, daqTime) + 2400000.5;
}

int32_t I3Time::yearOf(double modjulianday)
{
  double julianDay = modjulianday + 2400000.5;
  UTinstant i;
  i.j_date = julianDay;
  CalDate(&i);
  return i.year;
}

int32_t I3Time::DayOfYear(double modjulianday)
{
  UTinstant i;
  double julianDay = modjulianday + 2400000.5;
  i.j_date = julianDay;
  CalDate(&i);
  
  return i.day_of_year;
}

int32_t I3Time::DayOfYear(int64_t daqTime)
{
  int64_t tenthsOfNs = daqTime %((int64_t)1e10);
  int64_t daqSecs = (daqTime - tenthsOfNs)/((int64_t)1e10);
  int64_t daqSecsSinceDay = daqSecs % ((int64_t)(3600 * 24));
  int32_t day_of_year = (daqSecs - daqSecsSinceDay)/(3600 * 24) + 1;
  
  return day_of_year; 
}

template <class Archive>
void 
I3Time::serialize(Archive& ar, unsigned version)
{
  if (version>i3time_version_)
    log_fatal("Attempting to read version %u from file but running version %u of I3Time class.",version,i3time_version_);

  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("Year", year_);
  ar & make_nvp("DaqTime", daqTime_);
}

I3_SERIALIZABLE(I3Time);
