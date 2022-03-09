/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file GeoSelUtils.cxx
 * @version $Revision$
 * @date $Date$
 * @author olivas
 *
 * This class shows how to add a Selection to the event.
*/

// headers for this selection
#include "phys-services/geo-selector/GeoSelUtils.h"
#include "icetray/I3Logging.h"

#include <string>
#include <iostream>
#include <sstream>

using std::vector;
using std::string;
using std::ostringstream;
using std::endl;

namespace geo_sel_utils{
 
  const string COMMA(",");
  const string COLON(":"); 
  const string MINUS("-"); 
  const string SINGLE_DIGIT_INTEGERS("0123456789");

  template <class T> bool exists(const T& value, const vector<T>& v){
    for(typename vector<T>::const_iterator iter = v.begin(); iter != v.end(); ++iter)
      if(*iter == value) return true;
    return false;
  }

  template bool exists(const int&, const vector<int>&);

  template <class T> typename vector<T>::const_iterator 
  exists_at(const T& value, const vector<T>& v){
    for(typename vector<T>::const_iterator iter = v.begin(); iter != v.end(); ++iter){
      if(*iter == value) return iter;
    }
    return v.end();
  }
  
  bool good_input(const string& input_string){
    //check to that there's only numbers
    //and our two delimiters ',' and ':'
    for(string::const_iterator iter = input_string.begin(); 
	iter != input_string.end(); ++iter){
      if((COMMA.find(*iter) == string::npos) && 
	 (COLON.find(*iter) == string::npos) &&
	 (MINUS.find(*iter) == string::npos) &&
	 ((SINGLE_DIGIT_INTEGERS.find(*iter) == string::npos))){
	ostringstream os;
	os<<"Value ("<<*iter<<") "
	  <<"is failing one or more of the following conditions"<<endl;
	os<<" COMMA ("<<(COMMA.find(*iter) != string::npos)<<") "
	  <<" COLON ("<<(COLON.find(*iter) != string::npos)<<") "
	  <<" MINUS ("<<(MINUS.find(*iter) != string::npos)<<") "
	  <<" DIGIT ("<<(SINGLE_DIGIT_INTEGERS.find(*iter) != string::npos)<<") "<<endl;
	log_error("%s", os.str().c_str());
	return false;
      }
    }
    return true;
  }
  
  vector<int> make_good_strings(string to_use, string to_exclude){
    //There will be two delimiters ':' and ','
    //So something like "1:8,13" will ex/include strings 1 through 8 and 13
    //find all the commas first
    vector<int> strings_to_use(parse_string_list(to_use));
    vector<int> strings_to_exclude(parse_string_list(to_exclude));
    
    vector<int> strings_to_return;//We will return this
    for(vector<int>::iterator iter=strings_to_use.begin(); 
	iter != strings_to_use.end(); ++iter)
      if(!exists(*iter,strings_to_exclude)) strings_to_return.push_back(*iter);
    return strings_to_return;
  }
  
  vector<int> parse_string_list(const string& string_list){
    vector<int> final_string_list;
    if(string_list.size()){
      
      vector<string> string_list_buffer;//we'll need to clean this of ranges
      int n_commas(0);
      
      for(string::const_iterator iter = string_list.begin(); iter!= string_list.end(); ++iter)
	if(COMMA.find(*iter)!= string::npos) n_commas++;
      
      string::size_type this_pos(0);
      string::size_type last_pos(0);
      
      for(int i=0; i<=n_commas; i++){
	this_pos = string_list.find(',',last_pos);
	string_list_buffer.push_back(string_list.substr(last_pos,this_pos-last_pos));
	last_pos = this_pos+1;//need to skip over the comma
      }
      //Now need to go through the elements and expand the colons
      for(vector<string>::iterator iter = string_list_buffer.begin(); 
	  iter != string_list_buffer.end(); ++iter){
	string::size_type colon_pos = iter->find(':');
	//remove these from strings_to_use
	if(colon_pos == string::npos){
	  //convert to a number and push into final_string_list
	  final_string_list.push_back(atoi(iter->c_str()));
	}else{
	  string start_str = iter->substr(0,colon_pos);
	  string finish_str = iter->substr(colon_pos+1);
	  int start = atoi(start_str.c_str());
	  int finish = atoi(finish_str.c_str());
	  //cout<<*iter<<" "<<"start: "<<start<<" finish: "<<finish<<endl;
	  for(int i=start; i<=finish;i++){
	    final_string_list.push_back(i);
	  }
	}
      }
    }
    return final_string_list;
  }

  std::pair<double,double> detector_center(I3GeometryConstPtr geo, 
					   const vector<int>& goodStrings){

    vector<double> x;
    vector<double> y;
    I3OMGeoMap::const_iterator  i;
    vector<int>::const_iterator str_iter = goodStrings.begin();
    for(i=geo->omgeo.begin(); i!=geo->omgeo.end() && str_iter!=goodStrings.end(); i++){
      if(*str_iter == i->first.GetString()){
	x.push_back(i->second.position.GetX());
	y.push_back(i->second.position.GetY());
	str_iter++;
      }
    }

    vector<double>::iterator c_iter;

    double x_avg(0.);
    for(c_iter=x.begin(); c_iter!=x.end(); c_iter++)
      x_avg += *c_iter;
    x_avg /= static_cast<double>(x.size());

    double y_avg(0.);
    for(c_iter=y.begin(); c_iter!=y.end(); c_iter++)
      y_avg += *c_iter;
    y_avg /= static_cast<double>(y.size());

    std::pair<double,double> r(x_avg,y_avg);
    return r;
  }

  template<class T>
  boost::shared_ptr<I3Map<OMKey,T> >
  map_cleaner(boost::shared_ptr<const I3Map<OMKey,T> > m, vector<int> s){
    vector<int>::iterator j;
    boost::shared_ptr<I3Map<OMKey,T> > new_map; 
    typename I3Map<OMKey,T>::iterator i = m.begin();
    for(; i!=m.end();i++){
      bool good(false);
      for(j=s.begin(); j!=s.end(); j++){
	if(i->first.GetString() == *j){
	  good=true;
	  break;
	}
      }
      if(good) (*new_map)[i->first] = i->second;
    }
    return new_map;
  }

}//end namespace geo_sel_utilities

