#ifndef LARLITE_RESONANTRECO_CXX
#define LARLITE_RESONANTRECO_CXX

#include "ResonantReco.h"

namespace larlite {

  bool ResonantReco::initialize() {

    // Do all variable initialization you wish to do here.
  
    all_events = 0;

    return true;
  }
  
  bool ResonantReco::analyze(storage_manager* storage) {
  
    all_events++;
    
    // Do your event-by-event analysis here. This function is called for 
    // each event in the loop. You have "storage" pointer which contains 
    // event-wise data. To see what is available, check the "Manual.pdf":
    // http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
    // 
    // event_fifo* my_pmtfifo_v = (event_fifo*)(storage->get_data(DATA::PMFIFO));
    // if( event_fifo )
    //   std::cout << "Event ID: " << my_pmtfifo_v->event_id() << std::endl;
  
    return true;
  }

  bool ResonantReco::finalize() {

    std::cout << "All events : \t " << all_events << std::endl;

    // Do all variable finalization you wish to do here.
    // If you need, you can store your ROOT class instance in the output
    // if(_fout) { _fout->cd(); h1->Write(); }
    // else 
    //   print(MSG::ERROR,__FUNCTION__,"Did not find an output file pointer!!! File not opened?");
  
    return true;
  }

}
#endif
