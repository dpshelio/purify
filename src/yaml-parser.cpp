/** 
    YamlParser definition for the purify project
    @author Roland Guichard
    @version 1.0
 */

#include <typeinfo>
#include <iostream>
#include <fstream>
#include "yaml-cpp/yaml.h"
#include "yaml-parser.h"
#include <assert.h>

/** 
    YamlParser constructor definition
    
    @param filepath path to config file
*/
YamlParser::YamlParser (const std::string& filepath)
{
  // Setting the class variable
  this->filepath = filepath;
  // Reading the file
  this->readFile();
  // Run a batch of methods to parse the YAML and set the
  // class members accordingly
  this->setParserVariablesFromYaml();
}

/** 
    Class method for reading the config file
*/
void YamlParser::readFile ()
{
  YAML::Node config = YAML::LoadFile(this->filepath);
  // A bit of defensive programming
  assert(config.Type() == YAML::NodeType::Map);
  this->config_file = config;
}

/** 
    Class method for setting the class members from the YAML inputs
*/
void YamlParser::setParserVariablesFromYaml ()
{
  // Each of these methods correspond to blocks in the YAML
  this->parseAndSetGeneralConfiguration(this->config_file["GeneralConfiguration"]);
  this->parseAndSetMeasureOperators(this->config_file["MeasureOperators"]);
  this->parseAndSetSARA(this->config_file["SARA"]);
  this->parseAndSetAlgorithmOptions(this->config_file["AlgorithmOptions"]); 
}

/** 
    Class method for the YAML GeneralConfiguration block
*/
void YamlParser::parseAndSetGeneralConfiguration (YAML::Node generalConfigNode)
{  
  this->logging = generalConfigNode["logging"].as<std::string>();
  this->iterations = generalConfigNode["iterations"].as<int>();
  this->epsilonScaling = generalConfigNode["epsilonScaling"].as<int>();
  this->gamma = generalConfigNode["gamma"].as<std::string>();
  this->output_prefix = generalConfigNode["InputOutput"]["output_prefix"].as<std::string>();
  this->skymodel = generalConfigNode["InputOutput"]["skymodel"].as<std::string>();
  this->measurements = generalConfigNode["InputOutput"]["input"]["measurements"].as<std::string>();
  this->polarization_measurement = generalConfigNode["InputOutput"]["input"]["polarization_measurement"].as<std::string>();
  this->noise_estimate = generalConfigNode["InputOutput"]["input"]["noise_estimate"].as<std::string>();
  this->polarization_noise = generalConfigNode["InputOutput"]["input"]["polarization_noise"].as<std::string>();
}

/** 
    Class method for the YAML MeasureOperators block
*/
void YamlParser::parseAndSetMeasureOperators (YAML::Node measureOperatorsNode)
{
  this->Jweights = measureOperatorsNode["Jweights"].as<std::string>();
  this->wProjection = measureOperatorsNode["wProjection"].as<bool>();
  this->oversampling = measureOperatorsNode["oversampling"].as<float>();
  this->powMethod_iter = measureOperatorsNode["powMethod_iter"].as<int>();
  this->powMethod_tolerance = measureOperatorsNode["powMethod_tolerance"].as<float>();
  this->Dx = measureOperatorsNode["pixelSize"]["Dx"].as<double>();
  this->Dy = measureOperatorsNode["pixelSize"]["Dy"].as<double>();
  this->x = measureOperatorsNode["imageSize"]["x"].as<int>();
  this->y = measureOperatorsNode["imageSize"]["y"].as<int>();
  this->Jx = measureOperatorsNode["J"]["Jx"].as<unsigned int>();
  this->Jy = measureOperatorsNode["J"]["Jy"].as<unsigned int>();
  this->chirp_fraction = measureOperatorsNode["wProjection_options"]["chirp_fraction"].as<float>();
  this->kernel_fraction = measureOperatorsNode["wProjection_options"]["kernel_fraction"].as<float>();
}

/** 
    Class method for the YAML SARA block
*/
void YamlParser::parseAndSetSARA (YAML::Node SARANode)
{
  std::string values_str = SARANode["wavelet_dict"].as<std::string>();
  this->wavelet_basis = this->getWavelets(values_str);
  this->wavelet_levels = SARANode["wavelet_levels"].as<int>();
  this->algorithm = SARANode["algorithm"].as<std::string>();
}

/** 
    Class method for the YAML AlgorithmOptions block
*/
void YamlParser::parseAndSetAlgorithmOptions (YAML::Node algorithmOptionsNode)
{
  this->epsilonConvergenceScaling = algorithmOptionsNode["padmm"]["epsilonConvergenceScaling"].as<int>();
  this->realValueConstraint = algorithmOptionsNode["padmm"]["realValueConstraint"].as<bool>();
  this->positiveValueConstraint = algorithmOptionsNode["padmm"]["positiveValueConstraint"].as<bool>();
  this->mpiAlgorithm = algorithmOptionsNode["padmm"]["mpiAlgorithm"].as<std::string>();
  this->relVarianceConvergence = algorithmOptionsNode["padmm"]["relVarianceConvergence"].as<double>();  
  this->param1 = algorithmOptionsNode["pd"]["param1"].as<std::string>();
  this->param2 = algorithmOptionsNode["pd"]["param2"].as<std::string>();
}

/** 
    Class method for translating the YAML input wavelet definition into a vector
*/
std::vector<int> YamlParser::getWavelets(std::string values_str)
{
  // input - values_str
  // std::string values_str;
  // values_str = "1, 2, 4..6, 11..18, 24, 31..41"; //config["SARA"]["wavelet_dict"].as<std::string>();

  // Logic to extract the values as vectors
  std::vector<int> wavelets;
  std::string value2add;
  values_str.erase(std::remove_if(values_str.begin(), values_str.end(),
                                  [](char x){return std::isspace(x);}), values_str.end());
  // NOTE Maybe a while reststring and using find is better?
  for (int i=0; i <= values_str.size(); i++) {
    if (i == values_str.size() || values_str[i] == ','){
      wavelets.push_back(std::stoi(value2add));
      value2add = "";
    } else if (values_str[i] == '.') {
      // TODO throw exception if open ended: 9..
      // TODO throw if at the begining
      // TODO throw if 3 digits on side
      int n = values_str[i+3] == ',' ? 2 : 3;
      std::string final_value = values_str.substr(i+2, n);
      // TODO throw if final_value < start value
      for (int j=std::stoi(value2add); j <= std::stoi(final_value); j++ )
        wavelets.push_back(j);
      i += (n + 1);
      value2add = "";
    } else {
      value2add = value2add + values_str[i];
    }
  }

  return wavelets;
}

