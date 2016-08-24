#ifndef LARLITE_SINGLEPIONS_CXX
#define LARLITE_SINGLEPIONS_CXX

#include "SinglePions.h"

namespace larlite {

  bool SinglePions::initialize() {

    // Variable initialisation
    mcpart = 0;
    PionMom = 0;
    PionE = 0;
    PionTrackID = -99999;
    return true;
  }
// ---------------------------------------------------  
  bool SinglePions::analyze(storage_manager* storage) {
  
    // Event-by-Event analysis

    auto mc = storage->get_data<event_mctruth>("generator");
    auto mctrk_v = storage->get_data<larlite::event_mctrack>("mcreco");  	// this is a vector of MCtracks
    auto mcparticles = mc->at(0).GetParticles(); 

    //Reconstruction access
    
    //get handle to the association

    auto pfp_v = storage->get_data<event_pfpart>("pandoraCosmic");			// PandoraPFParticles

    event_track* ev_track = nullptr;
    auto const& ass_tracks = storage->find_one_ass(pfp_v->id(), ev_track, pfp_v->id().second);	// vector of track indices associated with the pfparticle...
    
    
    // interaction mode 
    bool decayPion = false;
    bool piInelasticPion = false;
    bool exitPion = false;


    //other variables
    int countDaughters = 0;
    double borderdist [6];
    std::string pionProcess = "UNKNOWN";

    for(auto par : mcparticles) {
      mcpart++;
      if (par.PdgCode() == 211) {
        auto pion = par;
        PionE = pion.Momentum().E();
        PionEnergy->Fill(PionE);
        PionP.SetXYZ(pion.Momentum().Px(), pion.Momentum().Py(), pion.Momentum().Pz());
        PionMom = PionP.Mag();
        PionMomentum->Fill(PionMom);
      }
    } 
  
    // For mctracks in the event
    if(mctrk_v->size() != 0) {
  
      for(auto mctrk : *mctrk_v)  {
        if(mctrk.Process() == "primary" && mctrk.PdgCode() == 211 && mctrk.size() != 0) {
          PionLength->Fill(length(mctrk));
          PionTrackID = mctrk.TrackID();
        
          borderdist[0] = fabs(mctrk.back().X());
          borderdist[1] = fabs(mctrk.back().X() - 2.*(larutil::Geometry::GetME()->DetHalfWidth()));
          borderdist[2] = fabs(mctrk.back().Y() - larutil::Geometry::GetME()->DetHalfHeight());
          borderdist[3] = fabs(mctrk.back().Y() + larutil::Geometry::GetME()->DetHalfHeight());
          borderdist[4] = fabs(mctrk.back().Z());
          borderdist[5] = fabs(mctrk.back().Z() - larutil::Geometry::GetME()->DetLength());
 	}
        if(mctrk_v->size() != 0 && mctrk.MotherTrackID() == PionTrackID) countDaughters++;
      }

      for(auto mctrk : *mctrk_v) {
 	if(mctrk.Process() != "primary" && mctrk.MotherTrackID() == PionTrackID) {
          pionProcess = mctrk.Process();
        }
      }
      // Interaction Modes
      for(auto mctrk : *mctrk_v) {
	if(mctrk.Process() == "Decay" && mctrk.MotherTrackID() == PionTrackID ) 	decayPion = true;
        if(mctrk.Process() == "pi+Inelastic" && mctrk.MotherTrackID() == PionTrackID ) 	piInelasticPion = true;
	if(minimumValue(borderdist) < 10. && countDaughters == 1)			exitPion = true;
      }

    }

    if(!decayPion && !piInelasticPion) {
      MinEndPointDist->Fill(minimumValue(borderdist));
     // std::cout << "--------Process: " << pionProcess << std::endl; 
    }

    if(exitPion) {
      ExitPionMom->Fill(PionMom);
      ExitPionEnergy->Fill(PionE);
    }

    if(decayPion) {
      DecayPionEnergy->Fill(PionE);
      DecayPionMom->Fill(PionMom);
    }
    if(piInelasticPion) {
      PiInelasticPionEnergy->Fill(PionE);
      PiInelasticPionMom->Fill(PionMom);
    }



    // print information desired

    //decide which events
    if(exitPion && !decayPion && !piInelasticPion) {
      // Event info
      std::cout << "\n----------------------------------------" << std::endl;
      std::cout << "Run   " << storage->run_id() << "  Event " << storage->event_id() << std::endl << std::endl;

      // Generator level info
      for(auto par : mcparticles) {
        std::cout << "MCPART " << par.StatusCode() << "\t PDG : " << par.PdgCode() <<  std::endl;
      }

      // Geant level info
      if(mctrk_v->size() != 0) {
        for(auto mctrk : *mctrk_v)  {
          if(mctrk.PdgCode() > 0) {
            std::cout << "mctrack " << mctrk.PdgCode() << "\t ID : " << mctrk.TrackID() <<  "\t Process : " << mctrk.Process() << " \t Mother ID : " << mctrk.MotherTrackID() <<  " \t Track length : " << length(mctrk)<< std::endl;
            if(mctrk.size() !=0) {
              std::cout << " \t \t\t \t Start in TPC: [" << mctrk.front().X() << ", " << mctrk.front().Y() << ", " << mctrk.front().Z()  << "] \t End in TPC: [" << mctrk.back().X() << ", " << mctrk.back().Y() << ", " << mctrk.back().Z() << "] " << std::endl;
            }
          }
        }
      }

      if(decayPion) 	std::cout << "\t-->DECAY \t Process : " << pionProcess		<< std::endl;
      if(piInelasticPion) std::cout << "\t -->Pi+INELASTIC \t Process : "	<< pionProcess 	<<  std::endl;
      if(exitPion) 	std::cout << "\t -->Pion EXITS TPC \t Process : " << pionProcess 	<< std::endl;
      if(!decayPion && !piInelasticPion && !exitPion) std::cout << "\t -->Unclassified \t Process : " 	<< pionProcess << std::endl;
    

    // Reconstruction info
    std::cout << "\n Reconstruction Info " << std::endl;

    for(auto pfpart : *pfp_v) {
    //  std::cout << "Pandora Particle " << pfpart.PdgCode() << std::endl;
      }
    std::cout << "pfp_v->size() : " << pfp_v->size() << std::endl;
    std::cout << "ass_tracks.size() " << ass_tracks.size() << std::endl;
    if(ass_tracks.size() != 0) {
      for(size_t pfp_i = 0; pfp_i < ass_tracks.size(); ++pfp_i) {
        auto const& pfpp = (*pfp_v)[pfp_i];
	auto const& track = (*ev_track)[pfp_i];
        std::cout << "  PFParticle @ index = " << pfp_i << " ... ID = " << pfpp.Self() << " ... pdgCode " << pfpp.PdgCode() << " ... is primary? " << pfpp.IsPrimary() << " ... # daughters " << pfpp.NumDaughters() << " ... Track Length [" << track.Length()<< "] " << std::endl;
	std::cout << " \t \t\t \t Start: [" <<  track.Vertex().X() << ", " << track.Vertex().Y() << ", " << track.Vertex().Z() << "] \t End: [ " << track.End().X()<< ", " << track.End().Y() << ", " << track.End().Z() << "] " << std::endl;
      }
    }
    
    if(!ev_track or (ev_track->size() == 0)) {
      std::cout << "No track associated with PFPart! " << std::endl;
    }
  }

    return true;
  } 
//----------------------------------------------------
  bool SinglePions::finalize() {

    // write trees, histograms etc

  DecayPionMom->SetFillColor(kRed);
  PiInelasticPionMom->SetFillColor(kGreen);
  ExitPionMom->SetFillColor(kBlue);

  InteractionEnergy->Add(ExitPionEnergy);
  InteractionEnergy->Add(DecayPionEnergy);
  InteractionEnergy->Add(PiInelasticPionEnergy);

  InteractionMom->Add(ExitPionMom);
  InteractionMom->Add(DecayPionMom);
  InteractionMom->Add(PiInelasticPionMom);

    if(_fout) { _fout->cd();
        PionLength->Write();
  	PionMomentum->Write();
        PionEnergy->Write();
        DecayPionEnergy->Write();
        DecayPionMom->Write();
        PiInelasticPionEnergy->Write();
        PiInelasticPionMom->Write();
        ExitPionEnergy->Write();
	ExitPionMom->Write();
	InteractionEnergy->Write();
	InteractionMom->Write();  
	EndPoint->Write();
	MinEndPointDist->Write();
  }
    std::cout << "\n******* Number of MCPART " << mcpart << std::endl;
    return true;
  }

}
#endif
