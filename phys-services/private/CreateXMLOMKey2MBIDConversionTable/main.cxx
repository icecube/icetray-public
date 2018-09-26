/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file CreateXMLOMKey2MBIDConversionTable/main.cxx
 * @version $Revision$
 * @date $Date$
 */

#include <stdexcept>
#include <string>
#include <boost/program_options.hpp>

#include <phys-services/I3FileOMKey2MBID.h>


int main(int argc, char **argv)
{
  int retVal = 0;

  std::string source;
  std::string dest;

  // declare the supported options
  boost::program_options::options_description desc;
  desc.add_options()
    ("help,h", "print help,")
    ("source,s",
     boost::program_options::value<std::string>(&source),
     "get conversion table from TXT file \"source\" and")
    ("dest,d",
     boost::program_options::value<std::string>(&dest)->default_value("./mainboard_ids.xml.gz"),
     "place output in XML file \"dest\".")
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
  } // try
  catch(std::exception& ex)
  {
    std::cerr << argv[0] << ": " << ex.what() << std::endl;
    std::cout << desc << std::endl;
    return(1);
  } // catch(std::exception& ex)
  
  // --------------------------------------------------------------------------
  
  // help is needed - print the supported options
  if(vm.count("help")) std::cout << desc << std::endl;
  else
  {
    try{ I3FileOMKey2MBID(source).DumpAsXML(dest); } // try
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
