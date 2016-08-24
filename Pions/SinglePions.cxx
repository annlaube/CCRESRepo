#ifndef LARLITE_SINGLEPIONS_CXX
#define LARLITE_SINGLEPIONS_CXX

#include "SinglePions.h"

namespace larlite {

  bool SinglePions::initialize() {

    // Variable initialisation
    mcpart = 0;
    PionMom = 0;
    PionE = 0;
    PionP;
    PionTrackID = -99999;
    return true;
  }
// ---------------------------------------------------  
  bool SinglePions::analyze(storage_manager* storage) {
  
   // Event-by-Event analysis

  auto mc = storage->get_data<event_mctruth>("generator");
  auto mctrk_v = storage->get_data<larlite::event_mctrack>("mcreco");  	// this is a vector of MCtracks
  auto mcshow_v = storage->get_data<event_mcshower>("mcreco");      		// this is a vector of MCshowers
  auto mcparticles = mc->at(0).GetParticles(); 
  auto pfp_v = storage->get_data<event_pfpart>("pandoraCosmic");			// PandoraPFParticles

  event_vertex* ev_vertex = nullptr;
  auto const& ass_vertex = storage->find_one_ass(pfp_v->id(), ev_vertex, "pandoraCosmic");

  event_track* ev_track = nullptr;
  auto const& ass_tracks = storage->find_one_ass(pfp_v->id(), ev_track, "pandoraCosmicKHit");	// vector of track indices associated with the pfparticle...
  
  // interaction mode 
  bool decayPion = false;
  bool piInelasticPion = false;
  bool exitPion = false;


  //other variables
  int countDaughters = 0;
  double borderdist [6];

  std::cout << "\n----------------------------------------" << std::endl;
  std::cout << "Run   " << storage->run_id() << "  Event " << storage->event_id() << std::endl << std::endl; 
  for(auto par : mcparticles) {
    std::cout << "MCPART " << par.StatusCode() << "\t PDG : " << par.PdgCode() <<  std::endl;
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

  if(mctrk_v->size() != 0) {
  
    for(auto mctrk : *mctrk_v)  {
   
      if(mctrk.PdgCode() < 10000) { 
        std::cout << "mctrack " << mctrk.PdgCode() << "\t ID : " << mctrk.TrackID() <<  "\t Process : " << mctrk.Process() << " \t Mother ID : " << mctrk.MotherTrackID() <<  " \t Track length : " << length(mctrk)<< std::endl;
        if(mctrk.size() !=0) {
          std::cout << " \t \t\t \t Start: [" << mctrk.front().X() << ", " << mctrk.front().Y() << ", " << mctrk.front().Z() << "] \t End: [" << mctrk.back().X() << ", " << mctrk.back().Y() << ", " << mctrk.back().Z() << "] " << std::endl;
        }     
      }

      if(mctrk.Process() == "primary" && mctrk.PdgCode() == 211) {
        PionLength->Fill(length(mctrk));
        PionTrackID = mctrk.TrackID();
      }
    }
  }
  
  for(auto mctrk : *mctrk_v) {
    if(mctrk.Process() == "Decay" && mctrk.MotherTrackID() == PionTrackID ) {
      std::cout << "\t---------------->DECAY PION \t decays to \t " << mctrk.PdgCode() << std::endl;
      decayPion = true;
    }
    if(mctrk.Process() == "pi+Inelastic" && mctrk.MotherTrackID() == PionTrackID ) {
      std::cout << "\t ----------------> Pi+ Inelastic Scattering " << std::endl;
      piInelasticPion = true;
    }    
  }

  for(auto mctrk : *mctrk_v) {
    if(mctrk_v->size() != 0 && mctrk.MotherTrackID() == PionTrackID) countDaughters++;
  } 

  if(mctrk_v->size() != 0 && mctrk_v->at(0).PdgCode() == 211 && mctrk_v->at(0).size() != 0) {
    std::cout << "End point : " << mctrk_v->at(0).back().X() << ", " << mctrk_v->at(0).back().Y() << ", " << mctrk_v->at(0).back().Z() << std::endl;
    EndPoint->Fill(mctrk_v->at(0).back().X(), mctrk_v->at(0).back().Y(), mctrk_v->at(0).back().Z());
    borderdist[0] = fabs(mctrk_v->at(0).back().X());
    //borderdist[0] = minX1;
    borderdist[1] = fabs(mctrk_v->at(0).back().X() - 2.*(larutil::Geometry::GetME()->DetHalfWidth()));
    //borderdist[1] = minX2;
    borderdist[2] = fabs(mctrk_v->at(0).back().Y() - larutil::Geometry::GetME()->DetHalfHeight());
    //borderdist[2] = minY1;
    borderdist[3] = fabs(mctrk_v->at(0).back().Y() + larutil::Geometry::GetME()->DetHalfHeight());
    //borderdist[3] = minY2;
    borderdist[4] = fabs(mctrk_v->at(0).back().Z());
    //borderdist[4] = minZ1;
    borderdist[5] = fabs(mctrk_v->at(0).back().Z() - larutil::Geometry::GetME()->DetLength());
    //borderdist[5] = minZ2;
    std::cout << "************** " << borderdist[0] << ", " <<borderdist[1] << ", " <<borderdist[2] << ", " <<borderdist[3] << ", " <<borderdist[4] << ", " <<borderdist[5]  << std::endl;
    std::cout << "************* MIN " << minimumValue(borderdist) << std::endl;
    if(minimumValue(borderdist) < 10. && countDaughters == 1) exitPion = true;
  }

  if(exitPion) {
    std::cout << "\n \t-----------------> neither Decay nor pi+inelastic, exits TPC " << std::endl;
    MinEndPointDist->Fill(minimumValue(borderdist));
    ExitPionMom->Fill(PionMom);
    ExitPionEnergy->Fill(PionE);
  }

  if(!decayPion && !piInelasticPion && !exitPion){
    std::cout << "!!!***!!! non decay, non inelasitc, non-exit " <<std::endl;
  }      

  if(decayPion) {
    DecayPionEnergy->Fill(PionE);
    DecayPionMom->Fill(PionMom);
  }
  if(piInelasticPion) {
    PiInelasticPionEnergy->Fill(PionE);
    PiInelasticPionMom->Fill(PionMom);
  }

  // get the tracks associated with PFParticles  

  for(auto pfpart : *pfp_v) {
    // std::cout << "Pandora Particle " << pfpart.PdgCode() << std::endl;
  }
  
  for(size_t pfp_i = 0; pfp_i < ass_vertex.size(); ++pfp_i) { 
    auto const& pfpp = (*pfp_v)[pfp_i];
    std::cout << "  PFParticle @ index = " << pfp_i << " ... ID = " << pfpp.Self() << " ... pdgCode " << pfpp.PdgCode() << " ... # daughters " << pfpp.NumDaughters() << std::endl; 
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
