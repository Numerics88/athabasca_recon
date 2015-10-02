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

#include "ReconConfiguration.hpp"
#include "io/MetaImageReader.hpp"
#include "bonelab/boost/property_tree/util.hpp"
#include "bonelab/boost/property_tree/conf_parser.hpp"
#include "bonelab/sysinfo.hpp"
#include <set>
#include <iostream>
#include <algorithm>

using boost::optional;

// Create constant lists for all the valid keys.
// This is somewhat of a hassle to maintain by hand, but helps greatly
// in preventing incorrect configuration files due to typos.

const int numberOfValidSectionKeys = 6;
const char* validSectionKeys[numberOfValidSectionKeys] = {
  "Input",
  "Output",
  "Projections",
  "Volume",
  "Reconstruction",
  "Software"
  };

const int numberOfValidInputKeys = 6;
const char* validInputKeys[numberOfValidInputKeys] = {
  "RawProjectionsFile",
  "AttenuationProjectionsFile",
  "FilteredProjectionsFile",
  "DarkFieldFile",
  "BrightFieldFile",
  "PostScanBrightFieldFile"  
  };

const int numberOfValidOutputKeys = 4;
const char* validOutputKeys[numberOfValidOutputKeys] = {
  "AttenuationProjectionsFile",
  "FilteredProjectionsFile",
  "VolumeFile",
  "AttenuationCorrectionsFile"
  };

const int numberOfValidProjectionsKeys = 9;
const char* validProjectionsKeys[numberOfValidProjectionsKeys] = {
  "DataType",
  "Dimensions",
  "NumberOfProjections",
  "ProjectionAt180",
  "PixelSize",
  "CenterPixelU",
  "OffsetV",
  "ReverseRotation",
  "ProjectionStride"
  };

const int numberOfValidVolumeKeys = 3;
const char* validVolumeKeys[numberOfValidVolumeKeys] = {
  "Dimensions",
  "VoxelSize",
  "Origin"
  };

const int numberOfValidReconstructionKeys = 14;
const char* validReconstructionKeys[numberOfValidReconstructionKeys] = {
  "BadPixelCorrection",
  "FlatFieldBadThreshold",
  "DarkFieldBadThreshold",
  "BeamPowerCorrection",
  "BeamPowerIndependentVariable",
  "BeamPowerDecayConstantTerm",
  "BeamPowerDecayLinearTerm",
  "ProjectionBackgroundEdgeWidth",
  "PixelInterpolation",
  "CalculateCenterOfRotation",
  "SmoothingFilter",
  "SmoothingFilterRadius",
  "SmoothingFilterFrequencies",
  "ScalingFactor"
  };

const int numberOfValidSoftwareKeys = 4;
const char* validSoftwareKeys[numberOfValidSoftwareKeys] = {
  "Engine",
  "Threads",
  "MaximumVolumeMemory",
  "FilteringModule"
  };


namespace athabasca_recon
  {

  void ReconConfiguration::ReadProjectionsInfo()
    {
    // The following types don't have to agree with what will actually be used
    // in the reconstruction.
    typedef int64_t TIndex;
    typedef double TSpace;

    optional<std::string> rawProjectionsDataFile = this->m_PropertyTree.get_optional<std::string>("Input.RawProjectionsFile");
    optional<std::string> attenuationProjectionsDataFile = this->m_PropertyTree.get_optional<std::string>("Input.AttenuationProjectionsFile");
    optional<std::string> filteredProjectionsDataFile = this->m_PropertyTree.get_optional<std::string>("Input.FilteredProjectionsFile");
    if ((bool(rawProjectionsDataFile) +
         bool(attenuationProjectionsDataFile) +
         bool(filteredProjectionsDataFile)) != 1)
      {
      this->Die("Require exactly one of Input.RawProjectionsFile,"
                " Input.AttenuationProjectionsFile and Input.FilteredProjectionsFile");
      }
    std::string metafilename;
    metafilename = rawProjectionsDataFile ? *rawProjectionsDataFile : metafilename;
    metafilename = attenuationProjectionsDataFile ? *attenuationProjectionsDataFile : metafilename;
    metafilename = filteredProjectionsDataFile ? *filteredProjectionsDataFile : metafilename;
    std::cout << "Reading information from " << metafilename << " .\n";

    boost::property_tree::ptree proj_file_config;
    MetaImageReader<TIndex,TSpace> reader;
    reader.SetFileName(metafilename);
    reader.GetInformation();
    proj_file_config.put("Projections.DataType", reader.GetDataType());
    bonelab::Tuple<2,TIndex> dims(reader.GetDimSize()[1], reader.GetDimSize()[2]);
    proj_file_config.put("Projections.Dimensions", reverse(dims));
    proj_file_config.put("Projections.NumberOfProjections", static_cast<int>(reader.GetDimSize()[0]));
    bonelab::Tuple<2,TSpace> pixelsize;
    // This is a bit of a mess: to need to consult this->m_PropertyTree here
    // to check if higher priority value for pixelsize is defined there.
    if (boost::optional<bonelab::Tuple<2,TSpace> > pixelsize_override
         = this->m_PropertyTree.get_optional<bonelab::Tuple<2,TSpace> >("Projections.PixelSize"))
      {
      pixelsize = reverse(*pixelsize_override);
      }
    else if (reader.HasElementSpacing())
      {
      pixelsize = bonelab::Tuple<2,TSpace>(reader.GetElementSpacing()[1], reader.GetElementSpacing()[2]);
      proj_file_config.put("Projections.PixelSize", reverse(pixelsize));
      }
    else
      {
      pixelsize = bonelab::Tuple<2,TSpace>::ones();
      proj_file_config.put("Projections.PixelSize", reverse(pixelsize));
      }
    if (reader.HasOffset())
      {
      TSpace center_pixel_U = -reader.GetOffset()[2]/pixelsize[1];
      proj_file_config.put("Projections.CenterPixelU", center_pixel_U);
      proj_file_config.put("Projections.OffsetV", reader.GetOffset()[1]);
      }
    merge(this->m_PropertyTree, proj_file_config);
    }


  void ReconConfiguration::VerifyConfiguration()
    {
      
    // This verification code is kind of ugly, but it does tend to
    // trap problems early and produces intelligible error messages.
      
    // The following types don't have to agree with what will actually be used
    // in the reconstruction.
    typedef int64_t TIndex;
    typedef double TSpace;
    typedef double TValue;

    // ======== Input section ========

    if (int(bool(this->m_PropertyTree.get_optional<std::string>("Input.RawProjectionsFile"))) + 
        int(bool(this->m_PropertyTree.get_optional<std::string>("Input.AttenuationProjectionsFile"))) + 
        int(bool(this->m_PropertyTree.get_optional<std::string>("Input.FilteredProjectionsFile"))) != 1)
      {
      this->Die("Require exactly one of Input.RawProjectionsFile,"
                " Input.AttenuationProjectionsFile and Input.FilteredProjectionsFile");
      }

    if (boost::optional<boost::property_tree::ptree&> child = this->m_PropertyTree.get_child_optional("Input"))
      { this->CheckForUnknown(*child, "Input", validInputKeys, numberOfValidInputKeys); }
      
    // ======== Output section ========

    if (this->m_PropertyTree.get_optional<std::string>("Input.AttenuationProjectionsFile") &&
        this->m_PropertyTree.get_optional<std::string>("Output.AttenuationProjectionsFile"))
      {
      this->Die("Cannot specify both Input.AttenuationProjectionsFile and "
                " Output.AttenuationProjectionsFile");
      }
    if (this->m_PropertyTree.get_optional<std::string>("Input.FilteredProjectionsFile") &&
        this->m_PropertyTree.get_optional<std::string>("Output.FilteredProjectionsFile"))
      {
      this->Die("Cannot specify both Input.FilteredProjectionsFile and "
                " Output.FilteredProjectionsFile");
      }
    if (this->m_PropertyTree.get_optional<std::string>("Input.FilteredProjectionsFile") &&
        this->m_PropertyTree.get_optional<std::string>("Output.AttenuationProjectionsFile"))
      {
      this->Die("Cannot specify both Input.FilteredProjectionsFile and "
                " Output.AttenuationProjectionsFile");
      }

    if (boost::optional<boost::property_tree::ptree&> child = this->m_PropertyTree.get_child_optional("Output"))
      { this->CheckForUnknown(*child, "Output", validOutputKeys, numberOfValidOutputKeys); }

    // ======== Projections section ========

    bonelab::Tuple<2,TIndex> proj_dims = reverse(RequireParameter<bonelab::Tuple<2,TIndex> >("Projections.Dimensions"));
    RequireParameter<int>("Projections.NumberOfProjections");
    RequireParameter<bonelab::DataType_t>("Projections.DataType");
    bonelab::Tuple<2,TSpace> proj_pixel_size = reverse(RequireParameter<bonelab::Tuple<2,TSpace> >("Projections.PixelSize"));
    OptionalParameter<TSpace>("Projections.CenterPixelU", (proj_dims[1]-1)/TSpace(2.0));
    OptionalParameter<TSpace>("Projections.OffsetV", -proj_pixel_size[0]*(proj_dims[0]-1)/TSpace(2.0));
    std::string rr_option = OptionalParameter("Projections.ReverseRotation", std::string("False"));
    if (rr_option != "True" && rr_option != "False")
      {
      this->Die("Unrecognized value for Reconstruction.ReverseRotation");
      }

    OptionalParameter("Projections.ProjectionAt180", std::string("True"));

    if (boost::optional<boost::property_tree::ptree&> child = this->m_PropertyTree.get_child_optional("Projections"))
      { this->CheckForUnknown(*child, "Projections", validProjectionsKeys, numberOfValidProjectionsKeys); }

    // ======== Volume section ========

    if (this->m_PropertyTree.get_optional<std::string>("Output.VolumeFile"))
      {
      bonelab::Tuple<3,TSpace> vol_spacing;
      // Default voxel same size as pixels IF the pixels are square.
      if (proj_pixel_size[0] == proj_pixel_size[1])
        {
        bonelab::Tuple<3,TSpace> default_voxel_size(proj_pixel_size[0], proj_pixel_size[0], proj_pixel_size[0]);
        vol_spacing = reverse(OptionalParameter("Volume.VoxelSize", reverse(default_voxel_size)));
        }
      else
        {
        vol_spacing = reverse(RequireParameter<bonelab::Tuple<3,TSpace> >("Volume.VoxelSize"));
        }
      bonelab::Tuple<3,TIndex> default_vol_dims(
                  TIndex(proj_dims[0]*proj_pixel_size[0]/vol_spacing[0] + 0.5),
                  TIndex(proj_dims[1]*proj_pixel_size[1]/vol_spacing[1] + 0.5),
                  TIndex(proj_dims[1]*proj_pixel_size[1]/vol_spacing[2] + 0.5));
      bonelab::Tuple<3,TIndex> vol_dims = reverse(OptionalParameter<bonelab::Tuple<3,TIndex> >(
                    "Volume.Dimensions", reverse(default_vol_dims)));
      // TO DO: replace the following with a vector operation.
      bonelab::Tuple<3,TSpace> vol_origin_default(
                                    TSpace(-0.5)*(vol_dims[0]-1)*vol_spacing[0],
                                    TSpace(-0.5)*(vol_dims[1]-1)*vol_spacing[1],
                                    TSpace(-0.5)*(vol_dims[2]-1)*vol_spacing[2]);
      OptionalParameter("Volume.Origin", reverse(vol_origin_default));
      }
    
    if (boost::optional<boost::property_tree::ptree&> child = this->m_PropertyTree.get_child_optional("Volume"))
      { this->CheckForUnknown(*child, "Volume", validVolumeKeys, numberOfValidVolumeKeys); }

    // ======== Reconstruction section ========
    
    if (this->m_PropertyTree.get_optional<std::string>("Input.RawProjectionsFile"))
      {
      std::string bpc_option = OptionalParameter<std::string>("Reconstruction.BadPixelCorrection", std::string("Averaging"));
      if (bpc_option != "Averaging" && bpc_option != "None")
        {
        this->Die("Unrecognized value for Reconstruction.BadPixelCorrection");
        }
      if (bpc_option != "None")
        {
        OptionalParameter<int>("Reconstruction.FlatFieldBadThreshold", int(10));
        }
      std::string norm_option = OptionalParameter<std::string>("Reconstruction.BeamPowerCorrection", std::string("None"));
      if (norm_option == "ZeroProjectionEdgeAttenuation")
        {
        this->m_PropertyTree.put("Reconstruction.BeamPowerCorrection",std::string("NullProjectionEdge"));
        norm_option = "NullProjectionEdge";
        }
      if (norm_option != "ConstantTotalAttenuation" &&
          norm_option != "Manual" &&
          norm_option != "BeforeAndAfterBrightField" &&
          norm_option != "NullProjectionEdge" &&
          norm_option != "None")
        {
        this->Die("Unrecognized value for Reconstruction.BeamPowerCorrection");
        }
      if (norm_option == "Manual")
        {
        OptionalParameter("Reconstruction.BeamPowerDecayConstantTerm", 0.0);
        boost::optional<double> term = this->m_PropertyTree.get_optional<double>("Reconstruction.BeamPowerDecayLinearTerm");
        if (!term)
          {
          this->Die("Reconstruction.BeamPowerCorrection=Manual requires that a value be set for Reconstruction.BeamPowerDecayLinearTerm");
          }
        }
      else if (norm_option == "BeforeAndAfterBrightField")
        {
        OptionalParameter("Reconstruction.BeamPowerDecayConstantTerm", 0.0);
        boost::optional<std::string> fn = this->m_PropertyTree.get_optional<std::string>("Input.PostScanBrightFieldFile");
        if (!fn)
          {
          this->Die("Reconstruction.BeamPowerCorrection=BeforeAndAfterBrightField requires that a value be set for Input.PostScanBrightFieldFile");
          }
        }
      else if (norm_option == "NullProjectionEdge")
        {
        OptionalParameter("Reconstruction.ProjectionBackgroundEdgeWidth", 10);
        if (this->m_PropertyTree.get_optional<std::string>("Reconstruction.BeamPowerDecayConstantTerm"))
          {
          this->Die("Reconstruction.BeamPowerDecayConstantTerm cannot be specified with Reconstruction.BeamPowerCorrection=NullProjectionEdge");
          }
        }
      else if (norm_option == "ConstantTotalAttenuation")
        {
        OptionalParameter("Reconstruction.BeamPowerDecayConstantTerm", 0.0);
        }
      }

    if (this->m_PropertyTree.get_optional<std::string>("Output.VolumeFile"))
      {
      std::string opt = OptionalParameter("Reconstruction.PixelInterpolation", std::string("BilinearWithFallback"));
      if (opt != "Bilinear" &&
          opt != "NearestNeighbor" &&
          opt != "BilinearWithFallback")
        {
        this->Die("Invalid value for Reconstruction.PixelInterpolation");
        }      
      }

    std::string sf_opt = OptionalParameter("Reconstruction.SmoothingFilter", std::string("Gaussian"));
    if (sf_opt != "None" &&
        sf_opt != "Gaussian" &&
        sf_opt != "TaperedCosineWindow")
      {
      this->Die("Invalid value for Reconstruction.SmoothingFilter");
      }
    if (sf_opt == "Gaussian")
      {
      OptionalParameter("Reconstruction.SmoothingFilterRadius", 0.5);
      if (get_optional<std::string>("Reconstruction.SmoothingFilterFrequencies"))
        {
        this->Die("Reconstruction.SmoothingFilterFrequencies cannot be used "
                  "with Gaussian.  Use Reconstruction.SmoothingFilterRadius instead.");
        }
      }
    else if (sf_opt == "TaperedCosineWindow")
      {
      RequireParameter<bonelab::Tuple<2,TValue> >("Reconstruction.SmoothingFilterFrequencies");
      if (get_optional<std::string>("Reconstruction.SmoothingFilterRadius"))
        {
        this->Die("Reconstruction.SmoothingFilterRadius cannot be used "
                  "with TaperedCosineWindow.  Use Reconstruction.SmoothingFilterFrequencies instead.");
        }
      }
    
    if (boost::optional<boost::property_tree::ptree&> child = this->m_PropertyTree.get_child_optional("Reconstruction"))
      { this->CheckForUnknown(*child, "Reconstruction", validReconstructionKeys, numberOfValidReconstructionKeys); }

    // ======== Software section ========

    std::string engine_option = OptionalParameter<std::string>("Software.Engine", std::string("MultiThreaded"));
    if (engine_option != "SingleThreaded" && engine_option != "MultiThreaded")
      {
      this->Die("Unrecognized value for Software.Engine");
      }
    if (engine_option == "MultiThreaded")
      {
      std::string threads_string = OptionalParameter<std::string>("Software.Threads", std::string("Automatic"));
      if (threads_string != "Automatic")
        {
        int threads = RequireParameter<int>("Software.Threads");
        if (threads < 1)
          {
          this->Die("Software.Threads must be a number greate than 0.");
          }
        }
      }

    if (this->m_PropertyTree.get_optional<std::string>("Output.VolumeFile"))
      { OptionalParameter("Software.MaximumVolumeMemory", std::string("Automatic")); }

    if (!this->m_PropertyTree.get_optional<std::string>("Input.FilteredProjectionsFile"))
      {
#ifdef USE_VDSP
      std::string opt = OptionalParameter("Software.FilteringModule", std::string("vDSP"));
#elif defined USE_FFTW
      std::string opt = OptionalParameter("Software.FilteringModule", std::string("FFTW"));
#else
// #warning No FFT library available.
      std::string opt = OptionalParameter("Software.FilteringModule", std::string("RealSpaceConvolution"));
#endif
      if (opt != "vDSP" && opt != "RealSpaceConvolution" && opt != "FFTW")
        {
        this->Die("Invalid value for Software.FilteringModule");
        }      
#ifndef USE_VDSP
      if (opt == "vDSP")
        {
        this->Die("vDSP library use requested, but Athabasca Recon has not been compiled with support for that library.");
        }      
#endif
#ifndef USE_FFTW
      if (opt == "FFTW")
        {
        this->Die("FFTW library use requested, but Athabasca Recon has not been compiled with support for that library.");
        }      
#endif
      }
    
    if (boost::optional<boost::property_tree::ptree&> child = this->m_PropertyTree.get_child_optional("Software"))
      { this->CheckForUnknown(*child, "Software", validSoftwareKeys, numberOfValidSoftwareKeys); }

    // ======== Unrecognized sections ========

    this->CheckForUnknown(this->m_PropertyTree, NULL, validSectionKeys, numberOfValidSectionKeys);

    }


  void ReconConfiguration::ReportConfiguration(std::ostream& os)
    {
    os << "\nComplete Configuration:\n"
       << "--------------------------------------------------------------------\n";
    write_conf(std::cout, this->m_PropertyTree);
    os << "--------------------------------------------------------------------\n";
    }


  void ReconConfiguration::ReadConfiguration
    (const char* config_file)
    {
    std::cout << "Reading configuration file " << config_file << " .\n";
    read_conf(config_file, this->m_PropertyTree);
    }

  int64_t ReconConfiguration::ParseMemoryValue(std::string key) const
    {
    int64_t systemMemory = bonelab::GetSystemTotalMemory();
    std::string optionText = this->m_PropertyTree.get<std::string>(key);
    if (optionText == "Automatic")
      {
      int64_t five_hundred_MB = 512*1024*1024;
      int64_t one_GB = 1024*1024*1024;
      return std::max<int64_t>(five_hundred_MB, systemMemory - one_GB);
      }
    else
      {
      int64_t factor;
      size_t pos;
      if ((pos = optionText.find("MB")) != std::string::npos)
        { factor = 1024*1024; }
      else if ((pos = optionText.find("GB")) != std::string::npos)
        { factor = 1024*1024*1024; }
      else
        {
        this->Die(format("Invalid units for %s. Must specify MB or GB.") % key);
        }
      std::string valueText = optionText.substr(0,pos);
      boost::trim(valueText);  
      int64_t value;
      try
        { value = boost::lexical_cast<int64_t>(valueText); }
      catch (...)
        {
        this->Die(format("Unable to parse %s numerical value.") % key);
        }
      value *= factor;
      bonelab_assert(value > 0);
      return value;
      }
    }
    

  template <typename T>
  T ReconConfiguration::RequireParameter(const char* key)
    {
    optional<std::string> present = this->m_PropertyTree.get_optional<std::string>(key);
    if (!present)
      { this->Die(format("Missing required parameter %s") % key); }
    optional<T> p = this->m_PropertyTree.get_optional<T>(key);
    if (!p)
      { this->Die(format("Error parsing configuration parameter %s") % key); }
    return *p;
    }


  template <typename T>
  T ReconConfiguration::OptionalParameter
    (
    const char* key,
    T def)
    {
    optional<std::string> present = this->m_PropertyTree.get_optional<std::string>(key);
    if (!present)
      {
      this->m_PropertyTree.put(key, def);
      return def;
      }
    optional<T> p = this->m_PropertyTree.get_optional<T>(key);
    if (!p)
      {
      this->Die(format("Error parsing configuration parameter %s") % key);
      }
    return *p;
    }

  void ReconConfiguration::CheckForUnknown
    (
    const boost::property_tree::ptree& section,
    const char* sectionKey,
    const char** validKeys,
    int numberOfValidKeys
    )
    {
    std::set<std::string> keyList(validKeys, validKeys + numberOfValidKeys);
    for (boost::property_tree::ptree::const_iterator it = section.begin(),
         end = section.end();
         it != end;
         ++it)
      {
      if (keyList.count(it->first) == 0)
        {
        if (sectionKey)
          {
          this->Die(format("Unrecognized key in configuration: %s.%s")
                      % sectionKey % it->first);
          }
        else
          {
          this->Die(format("Unrecognized section in configuration: %s")
                      % it->first);
          }
        }
      }
    }

  void ReconConfiguration::Die(std::string message) const
    {
    std::cerr << "Configuration Error: " << message << "\n";
    exit(-1);
    }

  void ReconConfiguration::Die(const boost::format& message) const
    {
    this->Die(message.str());
    }

  }  // namespace athabasca_recon
