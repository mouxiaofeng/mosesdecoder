/***********************************************************************
Moses - factored phrase-based language decoder
Copyright (C) 2009 University of Edinburgh

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
***********************************************************************/

#include <fstream>
#include "FeatureFunction.h"
#include "Model1.h"

namespace Josiah {

FeatureFunction::~FeatureFunction(){} // n.b. is pure virtual, must be empty

auto_ptr<FeatureRegistry> FeatureRegistry::s_instance;

const FeatureRegistry* FeatureRegistry::instance() {
  if (!s_instance.get()) {
    s_instance.reset(new FeatureRegistry());
  }
  return s_instance.get();
}

FeatureRegistry::FeatureRegistry() {
  m_names.push_back("dummy");
}


const std::vector<string>& FeatureRegistry::getFeatureNames() const {
  return m_names;
}


void FeatureRegistry::createFeatures(Sample& sample, std::vector<FeatureFunction*> features) {
  features.push_back(new DummyFeatureFunction());
}

void configure_features_from_file(const std::string& filename, feature_vector& fv){
  std::cerr << "Reading extra features from " << filename << std::endl;
  std::ifstream in(filename.c_str());
  // todo: instead of having this function know about all required options of
  // each feature, have features populate options / read variable maps /
  // populate feature_vector using static functions.
  po::options_description desc;
  desc.add_options()
    ("model1.table", "Model 1 table")
    ("model1.pef_column", "Column containing p(e|f) score")
    ("model1.pfe_column", "Column containing p(f|e) score");
  po::variables_map vm;
  po::store(po::parse_config_file(in,desc,true), vm);
  if (!vm["model1.pef_column"].empty() || !vm["model1.pfe_column"].empty()){
    boost::shared_ptr<external_model1_table> ptable;
    if (vm["model1.table"].empty())
      throw std::runtime_error("Requesting Model 1 features, but no Model 1 table given");
    else
      ptable.reset(new external_model1_table(vm["model1.table"].as<std::string>()));
    if (!vm["model1.pef_column"].empty())
      fv.push_back(feature_handle(new model1(ptable)));
    if (!vm["model1.pfe_column"].empty())
      fv.push_back(feature_handle(new model1_inverse(ptable))); 
  }
  in.close();
}
 
} //namespace
