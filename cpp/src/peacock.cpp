#include <algorithm>
#include <cassert>
#include <iostream>

#include "CB_Converter.hpp"
#include "args.hpp"

int main( int argc, char **argv ) {

  args::ArgumentParser parser(
    "This script is used to convert \"normally\" colored images to a              "
    "representation similar to what a colorblind viewer might see.  The           "
    "default action is to convert to all types of colorblindness (and to a        "
    "normal vision version). Converting to only a select type of colorblindness   "
    "can be accomplished with the CBTYPE parameter described below.\n"
    "\b\n"
    "The conversion processes and coefficients herein are used with permission    "
    "from Colblindor [http://www.color-blindness.com/] and were therein used with "
    "permission of Matthew Wickline and the Human-Computer Interaction Resource   "
    "Network [http://www.hcirn.com/] for non-commercial purposes.  As such, this  "
    "code may only be used for non-commercial purposes.                           ",
    "Valid entries for CBTYPE are:\n"
    "\b\n"
    " - Normal (normal vision)\n"
    " - Protanopia (red-blind)\n"
    " - Deuteranopia (green-blind)\n"
    " - Tritanopia (blue-blind)\n"
    " - Protanomaly (red-weak)\n"
    " - Deuteranomaly (green-weak)\n"
    " - Tritanomaly (blue-weak)\n"
    " - Achromatopsia (totally colorblind)\n"
    "\b\n"
  );
  args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
  args::Group arguments("arguments");
  args::ValueFlag<std::string> cbtype(parser, "CBTYPE", "The type of colorblindness, see below for valid entries (if not used: all)", {'c', "colorblindness"});
  args::Positional<std::string> infilename(parser, "infilename", "The path to the image to be processed.");

  // Parse command line arguments.
  try {
    parser.ParseCLI(argc, argv);
  // Output help and exit.
  } catch (args::Help) {
    std::cout << parser;
    return 0;
  // Output error message and exit.
  } catch (args::ParseError e) {
    std::cerr << e.what() << std::endl;
    std::cerr << parser;
    return 1;
  }

  assert( !args::get(infilename).empty() && "No input image filename specified, halting." );

  std::cout << "Processing: " << args::get(infilename) << std::endl;
  CB_Converter cb = CB_Converter( args::get(infilename) );

  // Perform a conversion to *all* representations of colorblindess (by default)
  // or to a user-specified form.
  if( !args::get(cbtype).compare("") ) {
    std::string conversion( "All" );
    cb.convert( conversion );
  } else {
    std::string s = args::get(cbtype);
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    s[0] = toupper(s[0]);
    cb.convert( s );
  }

  return 0;
}

