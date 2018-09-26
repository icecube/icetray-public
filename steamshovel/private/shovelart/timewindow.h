#ifndef I3_SHOVEL_ART_TIME_WINDOW_H
#define I3_SHOVEL_ART_TIME_WINDOW_H

#include <stdexcept>
#include <algorithm>

class TimeWindowError : public std::logic_error{
public:
	TimeWindowError( const std::string& what ) : std::logic_error(what) {}
};

/** A window of event time, in nanoseconds
 *
 * Besides a beginning, ending, and "current" time, a TimeWindow has a sub-window of time
 * called color time, which is used for applying color maps.
 *
 * Invariants of this class:
 *    start <= color_start < color_end <= end
 *    start <= current <= end
 *
 * The user interacts with this class through the TimelineWidget
 */
class TimeWindow{

public:
	enum TimePoint{
		START, COLOR_START, CURRENT, COLOR_END, END
	};

private:
	double t_start_;
	double color_start_;
	double current_;
	double color_end_;
	double t_end_;

public:
	TimeWindow() :
		t_start_(0), color_start_(0), current_(1), color_end_(1), t_end_(1)
	{}

	TimeWindow( double start, double end )
	{
		setWindow( start, start, start, end, end );
	}

	double getTime( TimePoint point ) const {
		switch(point){
			case START: return start();
			case COLOR_START: return colorStart();
			case CURRENT: return current();
			case COLOR_END: return colorEnd();
			case END: return end();
		}
		// shouldn't happen but mollifies gcc
		return end();
	}

	void setTime( TimePoint point, double time ){
		switch(point){
			case START: return setStart(time);
			case COLOR_START: return setColorStart(time);
			case CURRENT: return setCurrent(time);
			case COLOR_END: return setColorEnd(time);
			case END: return setEnd(time);
		}
	}

	void setWindow( double start, double cstart, double current, double cend, double end );

	double start() const { return t_start_; }
	void setStart(double t){
		setWindow( t, color_start_, current_, color_end_, t_end_ );
	}

	double end() const { return t_end_; }
	void setEnd(double t){
		setWindow( t_start_, color_start_, current_, color_end_, t );
	}

	void setBounds( double start, double end ){
		setWindow( start, color_start_, current_, color_end_, end );
	}

	double colorStart() const { return color_start_; }
	void setColorStart(double t){
		setWindow( t_start_, t, current_, color_end_, t_end_ );
	}

	double colorEnd() const { return color_end_; }
	void setColorEnd(double t){
		setWindow( t_start_, color_start_, current_, t, t_end_ );
	}

	void setColorBounds( double cstart, double cend ){
		setWindow( t_start_, cstart, current_, cend, t_end_ );
	}

	double current() const{ return current_; }
	void setCurrent( double t ){
		setWindow( t_start_, color_start_, t, color_end_, t_end_ );
	}

};

inline void TimeWindow::setWindow( double t0, double c0, double current, double cend, double tend ){
	// make sure color bounds are inside window bounds
	c0 = std::max(c0, t0);
	cend = std::min(cend, tend);

	if( t0 >= tend ){
		throw TimeWindowError( "Window start >= window end" );
	}
	if( c0 >= cend ){
		throw TimeWindowError( "Color start >= color end" );
	}

	// make sure current time is inside window bounds
	current = std::max(current, t0);
	current = std::min(current, tend);

	t_start_ = t0;
	color_start_ = c0;
	current_ = current;
	color_end_ = cend;
	t_end_ = tend;
}

#endif /* I3_SHOVEL_ART_TIME_WINDOW_H */
