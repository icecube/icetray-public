/**
 * copyright  (C) 2022
 * the icecube collaboration
 * $Id$
 *
 * @file MergeXMLOMKey2MBIDConversionTable/main.cxx
 * @version $Revision$
 * @date $Date$
 */

#include <stdexcept>
#include <string>
#include <vector>
#include <boost/program_options.hpp>

#include <phys-services/I3XMLOMKey2MBID.h>


int main(int argc, char **argv)
{
  int retVal = 0;

  std::vector<std::string> sources;
  std::string dest;

  // declare the supported options
  boost::program_options::options_description desc;
  desc.add_options()
    ("help,h", "print help,")
    ("source,s",
     boost::program_options::value<std::vector<std::string> >(&sources),
     "merge conversion tables from XML files \"source\" and")
    ("dest,d",
     boost::program_options::value<std::string>(&dest)->default_value("./mainboard_ids.xml.gz"),
     "write output to XML file \"dest\".")
    ;
  boost::program_options::positional_options_description p;
  p.add("source", -1);

  // parse options
  boost::program_options::variables_map vm;
  try
  {
    store(boost::program_options::command_line_parser(argc, argv)
          .options(desc).positional(p).run(), vm);
    notify(vm);
    if(sources.size() != 2)
      throw std::invalid_argument("need to define 2 source files");
  } // try
  catch(std::exception& ex)
  {
    std::cerr << argv[0] << ": " << ex.what() << std::endl;
    std::cout << "Usage: " << argv[0] << " [options] [SOURCE1] [SOURCE2]" << std::endl
              << desc << std::endl;
    return(1);
  } // catch(std::exception& ex)

  // --------------------------------------------------------------------------

  // help is needed - print the supported options
  if(vm.count("help"))
  {
    std::cout << "Usage: " << argv[0] << " [options] [SOURCE1] [SOURCE2]" << std::endl
              << desc << std::endl;
  }
  else
  {
    try{ I3XMLOMKey2MBID::Merge(dest, sources[0], sources[1]); } // try
    catch(std::exception& ex)
    {
      std::cerr << argv[0] << ": " << ex.what() << std::endl;
      retVal = 1;
    } // catch(std::exception& ex)
    catch(...)
    {
      std::cerr << argv[0] << ": unexpected exception" << std::endl;
      retVal = 1;
    } // catch(...)
  } // else

  return(retVal);
}
