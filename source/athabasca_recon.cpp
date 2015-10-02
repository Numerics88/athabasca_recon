/*
Copyright (C) 2011 Eric Nodwell
enodwell@ucalgary.ca

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** main file for Athabasca Recon */

#define BOOST_FILESYSTEM_VERSION 3

#include "version.h"
#include "ReconConfiguration.hpp"
#include "ReconstructionLauncher.hpp"
#include <boost/program_options.hpp>

using namespace athabasca_recon;
namespace program_options = boost::program_options;


// typedefs - giving us the flexibity to change our minds later.
// Right now these are hardcoded (the input type is not however).
// In a future version, the type may be selectable at run time.

typedef float TUseForAllFloatType;
typedef TUseForAllFloatType TSpace;        // type to use for real space dimensions
typedef int TIndex;                        // type to use for indexing Images
typedef TUseForAllFloatType TProjectionOutValue; // type to use for processed projection data
typedef TUseForAllFloatType TVolumeValue;  // type to use for volume data
typedef bonelab::Image<2,TProjectionOutValue,TIndex,TSpace> TProjectionOut;
typedef bonelab::Image<3,TVolumeValue,TIndex,TSpace> TVolume;


const char* usage =
"Usage: athabasca_recon [options] config_file\n"
"\n"
"Please refer to the athabasca recon manual.\n"
;


template <typename TProjectionInValue>
void create_and_launch(const ReconConfiguration& config)
  {
  typedef bonelab::Image<2,TProjectionInValue,TIndex,TSpace> TProjectionIn;
  ReconstructionLauncher<TProjectionIn,TProjectionOut,TVolume> launcher(config);
  launcher.Launch();
  }

 
int main(int argc, char **argv)
  {
  
  std::cout << "Athabasca Recon version " ATHABASCA_RECON_VERSION "\n"
            << "Copyright 2011, Eric Nodwell and Steven K. Boyd\n"
            << "http://bonelab.ucalgary.ca/\n\n";

  std::string config_file;

  // Declare a group of options that will be 
  // allowed only on command line
  program_options::options_description generic_options("Generic options");
  generic_options.add_options()
    ("help,h", "Produce help message")
    ("version,v", "Print version information")
    ("config,c", "Print the complete configuration")
    ;

  // Declare a group of positional arguments
  program_options::options_description hidden_options("Hidden options");
  hidden_options.add_options()
    ("config_file", program_options::value<std::string>(&config_file), "config file")
  ;
  program_options::positional_options_description options_pos;
  options_pos.add("config_file",1);

  // Combine options
  program_options::options_description cmdline_options;
  cmdline_options.add(generic_options).add(hidden_options);

  // Parse and store program options
  program_options::variables_map options_map;
  try
    {
    program_options::store (program_options::command_line_parser(argc, argv).options(cmdline_options).positional(options_pos).run(), options_map);
    program_options::notify (options_map);    
    }
  catch (program_options::error& err)
    {
    std::cerr << "Error parsing command line:\n"
              << err.what() << "\n"
              << "Use the --help option for more info.\n"; 
    return -1;
    }

  if (options_map.count("help"))
    {
    std::cout << usage << "\n" << generic_options;
    return 0;
    }

  if (options_map.count("version"))
    {
    // Don't actually need to do anything, since version is printed in any case.
    return 0;
    }

  if (options_map.count("config_file") == 0)
    {
    std::cerr << "You must specify a configuration file.\n"
              << "Use the --help option for more info.\n"; 
    return -1;
    }

  try
    {

    // Create a configuration object
    ReconConfiguration config;
    config.ReadConfiguration(config_file.c_str());
    config.ReadProjectionsInfo();
    config.VerifyConfiguration();
    config.ReportConfiguration(std::cout);

    // Don't proceed if just requesting config.
    if (options_map.count("config"))
      { return 0; }
    std::cout << "\n";

    switch (config.get<bonelab::DataType_t>("Projections.DataType"))
      {  
      case bonelab::BONELAB_INT8:
        create_and_launch<bonelab::int8_t>(config);
        break;
      case bonelab::BONELAB_UINT8:
        create_and_launch<bonelab::uint8_t>(config);
        break;
      case bonelab::BONELAB_INT16:
        create_and_launch<bonelab::int16_t>(config);
        break;
      case bonelab::BONELAB_UINT16:
        create_and_launch<bonelab::uint16_t>(config);
        break;
      case bonelab::BONELAB_INT32:
        create_and_launch<bonelab::int32_t>(config);
        break;
      case bonelab::BONELAB_UINT32:
        create_and_launch<bonelab::uint32_t>(config);
        break;
      case bonelab::BONELAB_FLOAT32:
        create_and_launch<float>(config);
        break;
      case bonelab::BONELAB_FLOAT64:
        create_and_launch<double>(config);
        break;
      default:
        throw_athabasca_exception("Invalid value for DataType");
      }

    }  // try
  catch (bonelab::bonelab_exception& e)
    {
    if (strlen(e.file()) != 0)
      { std::cerr << "Exception occured at source file " << e.file() << " line " << e.line() << ":\n"; }
    else
      { std::cerr << "Exception occured:\n"; }
    std::cerr << e.what() << "\n";
    exit(-1);
    }
  catch (std::exception& e)
    {
    std::cerr << "Exception occured:\n";
    std::cerr << e.what() << "\n";
    exit(-1);
    }
  catch (...)
    {
    std::cerr << "Unknown exception occured.\n";
    exit(-1);
    }

  }  // main
