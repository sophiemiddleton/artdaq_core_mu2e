#ifndef mu2e_artdaq_core_Overlays_CosmicVetoFragmentWriter_hh
#define mu2e_artdaq_core_Overlays_CosmicVetoFragmentWriter_hh

#include "artdaq-core/Data/Fragment.hh"
#include "cetlib/exception.h"

#include "mu2e-artdaq-core/Overlays/DetectorFragmentWriter.hh"

#include <iostream>
#include <ostream>
#include <vector>
#include <bitset>

namespace mu2e {
  class CosmicVetoFragmentWriter;
}

class mu2e::CosmicVetoFragmentWriter: public mu2e::DetectorFragmentWriter {

  public:

  CosmicVetoFragmentWriter(artdaq::Fragment & f); 
  ~CosmicVetoFragmentWriter(); 

  // Calorimeter-specific methods:
  mu2e::DetectorFragment::adc_t sipmID();
  mu2e::DetectorFragment::adc_t time();
  mu2e::DetectorFragment::adc_t numSamples();
  std::vector<mu2e::DetectorFragment::adc_t> cosmicADC();
  void printAll();

};

mu2e::CosmicVetoFragmentWriter::CosmicVetoFragmentWriter(artdaq::Fragment& f ) :
  DetectorFragmentWriter(f) {
}

mu2e::CosmicVetoFragmentWriter::~CosmicVetoFragmentWriter() {
}

mu2e::DetectorFragment::adc_t mu2e::CosmicVetoFragmentWriter::sipmID() {
  // The calorimeter data packets begin 8*16=128 bits after the beginning of the fragment data
  // Bits 0 to 15
  return mu2e::DetectorFragment::convertFromBinary(bitArray(dataBlockBegin()+8),127-16,127-0);
}

mu2e::DetectorFragment::adc_t mu2e::CosmicVetoFragmentWriter::time() {
  return mu2e::DetectorFragment::convertFromBinary(bitArray(dataBlockBegin()+8),127-32,127-16);
}

mu2e::DetectorFragment::adc_t mu2e::CosmicVetoFragmentWriter::numSamples() {
  return mu2e::DetectorFragment::convertFromBinary(bitArray(dataBlockBegin()+8),127-48,127-32);
}

std::vector<mu2e::DetectorFragment::adc_t> mu2e::CosmicVetoFragmentWriter::cosmicADC() {
  std::vector<adc_t> theVector;
  // Start after the 3rd 16-bit block of the first CRV DTC data packet:
  for(size_t adcIdx=0; adcIdx<numSamples(); adcIdx++) {
    theVector.push_back( *((adc_t const *)( (dataBlockBegin()+8+3) + adcIdx )) );
  }
  return theVector;
}

void mu2e::CosmicVetoFragmentWriter::printAll() {
  std::cout << "\t\t" << "SiPM ID:     " << (int)sipmID()  << std::endl;
  std::cout << "\t\t" << "Time:        " << (int)time()       << std::endl;
  std::cout << "\t\t" << "Num Samples: " << (int)numSamples() << std::endl;
  std::cout << "\t\t" << "ADC:         {[";
  std::vector<mu2e::DetectorFragment::adc_t> ADCarray = cosmicADC();
  for(size_t i=0; i<ADCarray.size(); i++) {
    std::cout << (int)ADCarray[i];
    if(i<ADCarray.size()-1) {
      std::cout << ",";
    }
  }
  std::cout << "]}" << std::endl;
}

#endif /* mu2e_artdaq_core_Overlays_CosmicVetoFragmentWriter_hh */
