#ifndef LARLITE_COUNT_CXX
#define LARLITE_COUNT_CXX

#include "count.h"
#include "DataFormat/mctruth.h"

namespace larlite {

    // This function is called in the begining of event loop
    bool count::initialize() {
      
      //initialise variables here
      
      all_events          = 0;
      nDeltaPP_total      = 0;
      nDeltaPP_nucltar    = 0;
      n3Track             = 0;
      nCCQE               = 0;
      nCCRes              = 0;
      nCCDRes             = 0;
      nThreeProng         = 0;
      
        
        
      std::cout << "Hello! Initialising! ... " << std::endl;

        return true;
    }
    
    // ---------------------------------------------------------------------------------------------------
    //This is run once for every "ARTEvent", basically just 1 readout
    bool count::analyze(storage_manager* storage) {
        
        // decide what to print
        bool print_CCQE = false;
        bool print_DeltaRes = true;
        
        // Interaction Classification
        bool CCQE = false;
        bool CCRes = false;
        bool CCDRes = false;
        bool ThreeProng = false;
        bool contains_DeltaPP = false;
        bool three_track = false;
        bool contains_NuclearClusterTarget = false;
      
        // Constants
        double M_proton = 0.938272;  //Proton Mass in GeV/c^2
        double M_pion   = 0.134977;  // Pion Mass in GeV/c^2
      
        // Final State Particle Count
        int nMuon       = 0;        // 13
        int nNuMu       = 0;        // 14
        int nDeltaPP    = 0;        // 2224
        int nProton     = 0;        // 2212
        int nNeutron    = 0;        // 2112
        int nPionP      = 0;        // 211
        int nPionM      = 0;        // -211
        int nPion0      = 0;        // 111
        int nKaon       = 0;        // 321
        int nFSPart     = 0;
        int nNuclearClusterTarget   = 0;
        
        
        //Final State kinetic information
        double lepton_E = 0;
        double lepton_P = 0;
        double proton_E = 0;
        double proton_P = 0;
        double pion_E   = 0;
        double pion_P   = 0;
        
        TVector3 lepton_p;
        TVector3 proton_p;
        TVector3 pion_p;
        TVector3 nucleon_p;         // target nucleon Momentum
        
        // Transverse Momentum variables
        TVector3 z_axis; z_axis.SetXYZ(0, 0, 1);
        TVector3 z_TT;
        double P_TT_p;
        double P_TT_pion;
        double delta_P_TT;
        double nucleon_E;
        double lepton_costheta;
        
        //get mc truth and mctracks
        auto mc = storage->get_data<event_mctruth>("generator");
        auto mctrk_v = storage->get_data<larlite::event_mctrack>("mcreco");        // this is a vector of MCtracks
        auto mcshow_v = storage->get_data<event_mcshower>("mcreco");      // this is a vector of MCshowers
        
        all_events++;
       
        //single mcnu
        auto& nu = mc->at(0).GetNeutrino();
        lepton_E = nu.Lepton().Momentum().E();
        lepton_p.SetXYZ(nu.Lepton().Momentum().Px(), nu.Lepton().Momentum().Py(), nu.Lepton().Momentum().Pz());
        lepton_costheta = nu.Lepton().Momentum().Pz()/lepton_p.Mag();
        lepton_P = lepton_p.Mag();
        
        if(nu.InteractionType() == 1001)  { CCQE = true; nCCQE++; }
        if(nu.InteractionType() == 1003)  { CCRes = true; nCCRes++ ;};
        
        /*std::cout << "*Generator neutrino \t" << nu.Nu().PdgCode() <<
                     "\t Status Code " << nu.Nu().StatusCode() <<
                     "\t Track ID " << nu.Nu().TrackId() <<
                     "\t CCNC " << nu.CCNC() <<
                     "\t InteractionType " << nu.InteractionType() <<
                     "\t Target " << nu.Target() <<
                     "\t HitNuc() " << nu.HitNuc() <<
                     "\t HitQuark " << nu.HitQuark() <<
                     std::endl;
        for(auto d: nu.Nu().Daughters()) {
            std::cout << "MC Neutrino daughters: " << d <<std::endl;
        }
        */
        
        //vector of mcpart
        auto& all_particles = mc->at(0).GetParticles();
    
        for(auto par: all_particles) {
            
            if(par.StatusCode() == 1) nFSPart++;
            if(par.PdgCode() == 2224){
                nDeltaPP++;
                contains_DeltaPP = true;
                nDeltaPP_total++;
            }
            if(par.PdgCode() == 13 && par.StatusCode() == 1) nMuon++;
            if(par.PdgCode() == 14 && par.StatusCode() == 1) nNuMu++;
            if(par.PdgCode() == 211 && par.StatusCode() == 1) nPionP++;
            if(par.PdgCode() == 2212 && par.StatusCode() == 1) nProton++;
            if(par.PdgCode() == 2112 && par.StatusCode() == 1) nNeutron++;
            if(par.PdgCode() == 111 && par.StatusCode() == 1) nPion0++;
            if(par.PdgCode() == -211 && par.StatusCode() == 1) nPionM++;
            if(abs(par.PdgCode()) == 321 && par.StatusCode() == 1) nKaon++;
            if(par.PdgCode() == 2000000300 && par.StatusCode() == 16) nNuclearClusterTarget++;
        }
        
        if((nProton + nMuon + nPionP + nPionM + nKaon) == 3) {
            three_track = true;
            n3Track++;
        }
        
        if(CCRes && three_track && nPionP == 1 && nProton == 1 && nMuon == 1 && nNeutron == 0 && nPion0 == 0 && nFSPart == 3){
            ThreeProng = true;
            nThreeProng++;
        }
        
        if(ThreeProng && contains_DeltaPP) {
            nCCDRes++;
            CCDRes = true;
        }
        
        if(contains_DeltaPP && nNuclearClusterTarget != 0){
            nDeltaPP_nucltar++;
            contains_NuclearClusterTarget = true;
        }
        
        
        if(print_CCQE) {
        if(CCQE) {
            std::cout <<"\t -------------------------------------------------------------------------------" <<std::endl;
            std::cout << " CCQE event " << std::endl;
            std::cout << "\t Run   " << storage->run_id() << std::endl;
            std::cout << "\t Event " << storage->event_id() << std::endl;
            std::cout << "\t Interaction Type (1001 CCQE) : " << nu.InteractionType() << std::endl;
            for(auto par: all_particles) {
                std::cout <<"\t * \tParticle " << par.PdgCode() <<
                "\t\t Status Code " << par.StatusCode() <<
                "\t Track ID " << par.TrackId() <<
                "\t Mother Track ID " << par.Mother() <<
                "\t End Process " << par.EndProcess() <<
                "\t Energy " << par.Momentum().E() <<
                std::endl;
                
                if(par.PdgCode() == 2212 && par.StatusCode() == 1) {
                    proton_E = par.Momentum().E();
                    proton_p.SetXYZ(par.Momentum().Px(), par.Momentum().Py(), par.Momentum().Pz());
                    h_proton_E_CCQE->Fill(proton_E);
                }
            }
        }
        }
        
        if(print_DeltaRes) {
        if(CCDRes) {
            std::cout <<"\t -------------------------------------------------------------------------------" <<std::endl;
            std::cout << " THREE TRACK DELTA++ EVENT WITH THREE FSPart\n" << std::endl;
            std::cout << "\t Run   " << storage->run_id() << std::endl;
            std::cout << "\t Event " << storage->event_id() << std::endl;
            std::cout << "\t Interaction Type (1003 CCRes_ProtonPiPlus, 1091 CCDIS : " << nu.InteractionType() << std::endl;
            for(auto par: all_particles) {
                std::cout <<"\t * \tParticle " << par.PdgCode() <<
                            "\t\t Status Code " << par.StatusCode() <<
                            "\t Track ID " << par.TrackId() <<
                            "\t Mother Track ID " << par.Mother() <<
                            "\t End Process " << par.EndProcess() <<
                            "\t Energy " << par.Momentum().E() <<
                            std::endl;
                if(par.PdgCode() == 2212 && par.StatusCode() == 1) {
                    proton_E = par.Momentum().E();
                    proton_p.SetXYZ(par.Momentum().Px(), par.Momentum().Py(), par.Momentum().Pz());
                    proton_P = proton_p.Mag();
                }
                if(par.PdgCode() == 211 && par.StatusCode() == 1) {
                    pion_E = par.Momentum().E();
                    pion_p.SetXYZ(par.Momentum().Px(), par.Momentum().Py(), par.Momentum().Pz());
                    pion_P = pion_p.Mag();
                }
                if(par.StatusCode() == 11) {
                    nucleon_E = par.Momentum().E();
                    nucleon_p.SetXYZ(par.Momentum().Px(), par.Momentum().Py(), par.Momentum().Pz());
                }
            }
            
            z_TT = (z_axis.Cross(lepton_p));
            z_TT = (1/(z_axis.Cross(lepton_p)).Mag())*z_TT;
            
            P_TT_p = proton_p.Dot(z_TT);
            P_TT_pion = pion_p.Dot(z_TT);
            delta_P_TT = P_TT_p + P_TT_pion;
            
            std::cout << "\n \t Initial States " << std::endl;
            std::cout << "\t neutrino energy : " << nu.Nu().Momentum().E() << std::endl;
            std::cout << "\t nucleon target energy : " << nucleon_E << ", \t momentum :" << nucleon_p.Mag() << std::endl;
            
            std::cout << "\n \t Final States" <<std::endl;
            std::cout << "\t Lepton Energy : " << lepton_E << std::endl;
            std::cout << "\t Proton Energy : " << proton_E << std::endl;
            std::cout << "\t Pion Energy : " << pion_E << std::endl;
            std::cout << "\t lepton_p : [" << lepton_p.X()  << ", " << lepton_p.Y() << ", " << lepton_p.Z() << "], \t lepton_p.Mag() : " << lepton_p.Mag() << std::endl;
            std::cout << "\t proton_p : [" << proton_p.X()  << ", " << proton_p.Y() << ", " << proton_p.Z() << "], \t proton_p.Mag() : " << proton_p.Mag() << std::endl;
            std::cout << "\t pion_p : [" << pion_p.X()  << ", " << pion_p.Y() << ", " << pion_p.Z() << "], \t pion_p.Mag() : " << pion_p.Mag() <<  std::endl;
            std::cout << "\t z_TT : [" << z_TT.X() << ", " << z_TT.Y() << ", " << z_TT.Z() << "], \t Mag " << z_TT.Mag() << std::endl;
            std::cout << "\t z_TT.lepton_p " << z_TT.Dot(lepton_p) << ", \t z_TT.z_axis " << z_TT.Dot(z_axis) << std::endl;
            std::cout << "\n \t P_TT_p : " << P_TT_p << std::endl;
            std::cout << "\t P_TT_pion : " << P_TT_pion << std::endl;
            std::cout << "\t delta_P_TT : " << delta_P_TT << std::endl;
            
            std::cout << std::endl;
            for(auto mctrk : *mctrk_v) {
                std::cout << "Geant Track with PDG : " << mctrk.PdgCode() << " \t Start in TPC? " << StartInTPC(mctrk) << " \t Track Length : " << length(mctrk) << std::endl;
            }
            
            std::cout << "\n \t --------------------------------------------------------------------------\n\n" << std::endl;
        }
        }
    
        
        //Fill Histograms
        
        if(contains_DeltaPP){
            nFinalState->Fill(nFSPart);
            //h_lepton_E->Fill(lepton_E);
        }
        if(contains_DeltaPP && contains_NuclearClusterTarget){
            nFinalStateCluster->Fill(nFSPart);
        }
        
        if(contains_DeltaPP && contains_NuclearClusterTarget == false){
            nFinalStateNeutron->Fill(nFSPart);
        }
        
        if(contains_DeltaPP && three_track){
            nFinalStates3Tracks->Fill(nFSPart);
            n3TrackNeutrons->Fill(nNeutron);
            //h_lepton_E->Fill(lepton_E);
        }
        
        if(CCDRes) {       // CCDRes events (charged current Delta Resonance)
            h_lepton_E_CCDRes->Fill(lepton_E);
            h_lepton_P_CCDRes->Fill(lepton_P);
            h_proton_KE_CCDRes->Fill(proton_E-M_proton);
            h_proton_P_CCDRes->Fill(proton_P);
            h_pion_KE_CCDRes->Fill(pion_E-M_pion);
            h_pion_P_CCDRes->Fill(pion_P);
            h_delta_P_TT->Fill(delta_P_TT);
            h_lepton_costheta_CCDRes->Fill(lepton_costheta);
        }
            
        if(CCQE) {
            h_lepton_E_CCQE->Fill(lepton_E);
            h_lepton_costheta_CCQE->Fill(lepton_costheta);
        }
        
        
    return true;
  }

    // ---------------------------------------------------------------------------------------------------
    //This is run once at the end of the job
    bool count::finalize() {

    if(_fout) { _fout->cd();
        nFinalState->Write();
        nFinalStateCluster->Write();
        nFinalStateNeutron->Write();
        FSPdg->Write();
        nFinalStates3Tracks->Write();
        n3TrackNeutrons->Write();
        
        h_lepton_E_CCQE->Write();
        h_proton_E_CCQE->Write();
        h_lepton_costheta_CCQE->Write();
        
        h_lepton_E_CCDRes->Write();
        h_lepton_P_CCDRes->Write();
        h_proton_KE_CCDRes->Write();
        h_proton_P_CCDRes->Write();
        h_pion_KE_CCDRes->Write();
        h_pion_P_CCDRes->Write();
        h_lepton_costheta_CCDRes->Write();
    
        h_delta_P_TT->Write();}
        
        
    //
    //else
    //    print(MSG::ERROR,__FUNCTION__,"Did not find an output file pointer!!! File not opened?");
    //
        
        std::cout << "Hello! Finalising! ..." << std::endl;
        std::cout << "all_events : \t\t\t\t\t" << all_events << std::endl;
        std::cout << "CCQE events : \t\t\t\t\t" << nCCQE << std::endl;
        std::cout << "CCResProtonPi+ events : \t\t\t" << nCCRes <<std::endl;
        std::cout << "CCRes with 3 tracks : \t\t\t\t" << nThreeProng << std::endl;
        std::cout << "CCRes with 3 tracks, Delta Resonance : \t\t" << nCCDRes << std::endl;
        std::cout << "events containing nDeltaPP : \t\t\t" << nDeltaPP_total << std::endl;
        std::cout << "nDeltaPP with nuclear Cluster Target: \t\t" << nDeltaPP_nucltar << std::endl;
        std::cout << "Event with 3 charged Tracks : \t\t\t" << n3Track << std::endl;
        
        std::cout << "\n--------------------------------------------------------\nCCDRes events" << std::endl;
        std::cout << "Average Lepton Momentum " << h_lepton_P_CCDRes->GetMean() << std::endl;
        std::cout << "Average Proton Momentum " << h_proton_P_CCDRes->GetMean() << std::endl;
        std::cout << "Average Pion Momentum " << h_pion_P_CCDRes->GetMean() << std::endl;
        
        std::cout << "---------------------------------------------------------" << std::endl;
      
    return true;
  }

}
#endif
