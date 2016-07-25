#ifndef LARLITE_RESONANTRECO_CXX
#define LARLITE_RESONANTRECO_CXX

#include "ResonantReco.h"

namespace larlite {

  bool ResonantReco::initialize() {

    // Do all variable initialization you wish to do here.
  
    all_events              = 0;
    CCRES_pion_all          = 0;
    CCRES_1pion             = 0;

    return true;
  }
 
// ---------------------------------------------------------------------------------------------
    
  bool ResonantReco::analyze(storage_manager* storage) {
      
      auto mc = storage->get_data<event_mctruth>("generator");
      auto mctrk_v = storage->get_data<larlite::event_mctrack>("mcreco");
      auto& nu = mc->at(0).GetNeutrino();
      auto& mcparticles = mc->at(0).GetParticles();
  
      all_events++;
      
      int npions = 0;
      bool contains_pion = false;
      bool contains_one_pion = false;
      
      for(auto par : mcparticles) {
          if(par.PdgCode() == 211 && nu.InteractionType() == 1003) {
              contains_pion = true;
              CCRES_pion_all++;
              npions++;
          }
      }
      
      if(npions == 1) {
          contains_one_pion = true;
          CCRES_1pion++;
      }
      
      
      
    
      
    return true;
  }

// ---------------------------------------------------------------------------------------------
    
  bool ResonantReco::finalize() {

    std::cout << "All events : \t\t " << all_events << std::endl;
    std::cout << "CCRES_pion_all : \t " << CCRES_pion_all << std::endl;
    std::cout << "CCRES_1pion : \t\t " << CCRES_1pion << std::endl;
      
    // Do all variable finalization you wish to do here.
    // If you need, you can store your ROOT class instance in the output
    // if(_fout) { _fout->cd(); h1->Write(); }
    // else 
    //   print(MSG::ERROR,__FUNCTION__,"Did not find an output file pointer!!! File not opened?");
  
    return true;
  }

}
#endif
