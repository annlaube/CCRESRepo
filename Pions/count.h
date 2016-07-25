/**
 * \file count.h
 *
 * \ingroup CountDelta
 * 
 * \brief Class def header for a class count
 *
 * @author laube
 */

/** \addtogroup CountDelta

    @{*/

#ifndef LARLITE_COUNT_H
#define LARLITE_COUNT_H

#include "Analysis/ana_base.h"
#include "DataFormat/mcpart.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/mctrack.h"
#include "DataFormat/mcshower.h"
#include "LArUtil/Geometry.h"
#include <TH1.h>

namespace larlite {
  /**
     \class count
     User custom analysis class made by SHELL_USER_NAME
   */
  class count : public ana_base{
  
  public:

    /// Default constructor
    count(){ _name="count"; _fout=0;}

    /// Default destructor
    virtual ~count(){}

    /** IMPLEMENT in count.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in count.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in count.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();
      
      int all_events        = 0;
      int nDeltaPP_total    = 0;
      int nDeltaPP_2p1pi0   = 0;
      int nDeltaPP_nucltar  = 0;
      int n3Track           = 0;
      int nCCQE             = 0;
      int nCCRes            = 0;
      int nCCDRes           = 0;
      int nThreeProng       = 0;
      
      TH1F *nFinalState             = new TH1F("nFinalState", "Number of Final State Particles", 40, 0, 40);
      TH1F *nFinalStateCluster      = new TH1F("nFinalStateCluster", "N of FS Particles in Int w Nuclear Cluster Target" , 40, 0, 40);
      TH1F *nFinalStateNeutron      = new TH1F("nFinalStateNeutron", "N of FS Particles in Int w Neutron FS", 40, 0, 40);
      
      TH1F *FSPdg                   = new TH1F("FSPdg", "PDG of Final State Particles", 250, 2000, 2500);
      TH1F *nFinalStates3Tracks     = new TH1F("nFinalStates3Tracks", "no of FS Particles in 3 Track events", 40, 0, 40);
      TH1F *n3TrackNeutrons         = new TH1F("n3TrackNeutrons", "no of neutrons in 3 track event", 30, 0, 30);
      
      TH1F *h_lepton_E_CCQE         = new TH1F("h_lepton_E_CCQE", "Lepton Energy in CCQE event" , 20, 0, 2);
      TH1F *h_proton_E_CCQE         = new TH1F("h_proton_E_CCQE", "Proton Energy in CCQE event", 20, 0, 0);
      TH1F *h_lepton_costheta_CCQE  = new TH1F("h_lepton_costheta_CCQE", "Lepton cosine theta in CCQE (angle to z axis", 20, -1, 1);
      
      
      TH1F *h_lepton_E_CCDRes       = new TH1F("h_lepton_E_CCDRes", "Lepton Energy in CCDRes", 20, 0, 2);
      TH1F *h_lepton_P_CCDRes       = new TH1F("h_lepton_P_CCDRes", "Lepton Momentum in CCDRes", 20, 0, 0);
      TH1F *h_lepton_costheta_CCDRes= new TH1F("h_lepton_costheta_CCDRes", "Lepton cosine theta in CCDRes (angle to z axis)", 20, -1, 1);
      TH1F *h_proton_KE_CCDRes      = new TH1F("h_proton_E_CCDRes", "Proton Kinetic Energy in CCDRes", 20, 0, 2);
      TH1F *h_proton_P_CCDRes       = new TH1F("h_proton_P_CCDRes", "Proton Momentum in CCDRes", 60, 0, 3);
      TH1F *h_pion_KE_CCDRes         = new TH1F("h_pion_E_CCDRes", "Pion Kinetic Energy in CCDRes", 20, 0, 2);
      TH1F *h_pion_P_CCDRes         = new TH1F("h_pion_P_CCDRes", "Pion Momentum in CCDRes", 20, 0,1);
      
      TH1F *h_delta_P_TT            = new TH1F("h_delta_P_TT", "Double-Transverse Momentum Imbalance", 20, 0,0 );
      
  private:
      // mctrack length
      double length(larlite::mctrack mctrk) {
          double len = 0;
          bool first = true;
          TVector3 disp;
          for(auto step : mctrk) {
              if(first) {
                  first = false;
                  disp = {step.X(), step.Y(), step.Z()};
              }
              else {
                  TVector3 pos(step.X(), step.Y(), step.Z());
                  disp -= pos;
                  len += disp.Mag();
                  disp = pos;
              }
          }
          return len;
      }
      
      // mctrack start in TPC?
      bool StartInTPC(larlite::mctrack mctrk){
          if(mctrk.Start().X() < 0 || mctrk.Start().X() > 2*(larutil::Geometry::GetME()->DetHalfWidth()) ||
             mctrk.Start().Y() < -(larutil::Geometry::GetME()->DetHalfHeight()) || mctrk.Start().Y() > larutil::Geometry::GetME()->DetHalfHeight() ||
             mctrk.Start().Z() < 0 || mctrk.Start().Z() > larutil::Geometry::GetME()->DetLength()) {
              return false;
          }
          else return true;
      }


      
  protected:
    
  };
}
#endif

//**************************************************************************
// 
// For Analysis framework documentation, read Manual.pdf here:
//
// http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
//
//**************************************************************************

/** @} */ // end of doxygen group 
