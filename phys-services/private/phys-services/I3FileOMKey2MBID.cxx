/*
 * class:  I3FileOMKey2MBID
 *
 * Version: $Id$
 *
 * (c) IceCube Collaboration
 */

#include <phys-services/I3FileOMKey2MBID.h>

#include <fstream>
#include <sstream>
#include <cassert>

#include <phys-services/I3XMLOMKey2MBID.h>




I3FileOMKey2MBID::I3FileOMKey2MBID(const std::string& infile)
{
  std::ifstream fin(infile.c_str());
  if(!fin)
    {
      log_fatal("Cannot find the DOMId to mainboard mapping file (%s)",
		infile.c_str());
      log_fatal("cannot find the DOMid to mainboard mapping file");
    }
  std::string omkey;
  std::string domid;
  std::string name;
  long long int mbid;
  while(fin.peek()!=EOF)
    if(fin.peek() == '#')
      {
	log_debug("comment line");
	fin.ignore(255,'\n');
      }
    else
      {
	fin>>omkey>>domid>>name>>std::hex>>mbid;
	log_debug("OMKey:%s DomID: %s, Name:%s, Mainboard ID: %lld",
		  omkey.c_str(),
		  domid.c_str(),
		  name.c_str(),
		  mbid);
	OMKey key = OMKeyize(omkey);
	mbid2omkey_[mbid] = key;
	omkey2mbid_[key] = mbid;
	fin.ignore(255,'\n');
      }
}


I3FileOMKey2MBID::~I3FileOMKey2MBID()
{
}


/**
 * @todo This is an ugly mess.
 */
OMKey I3FileOMKey2MBID::OMKeyize(const std::string& key) 
{
  if(key.size()==4 && key.find('-') == key.npos){
    //preserve mysterious old logic
    char om_string_array[3];
    om_string_array[0] = key[0];
    om_string_array[1] = key[1];
    om_string_array[2] = '\0';
    int om_string = atoi(om_string_array);
    int om_num= -1;
    if(key[2] == 'A'){
      if(key[3] == '1')
        om_num = 61;
      else if(key[3] == '2')
        om_num = 62;
      else
        log_fatal("bad om key: %s", key.c_str());
    }
    else if(key[2] == 'B'){
      if(key[3] == '1')
        om_num = 63;
      else if(key[3] == '2')
        om_num = 64;
      else
        log_fatal("bad om key: %s", key.c_str());
    }
    assert(om_num > 0);
    return OMKey(om_string,om_num);
  }
  
  int string=0;
  unsigned int om=0;
  unsigned char pmt=0;
  char dummy;
  std::istringstream ss(key);
  ss >> string >> dummy;
  if(ss.fail() || ss.eof() || dummy!='-')
    log_fatal("bad om key: %s", key.c_str());
  ss >> om;
  if(ss.fail())
    log_fatal("bad om key: %s", key.c_str());
  if(!ss.eof()){
    ss >> dummy >> pmt;
    if(ss.fail())
      log_fatal("bad om key: %s", key.c_str());
  }
  return OMKey(string,om,pmt);
}


void I3FileOMKey2MBID::DumpAsXML(const std::string& outfile) const
{
  I3XMLOMKey2MBID::Dump(outfile, mbid2omkey_);
}
