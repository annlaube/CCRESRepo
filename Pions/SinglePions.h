/**
 * \file SinglePions.h
 *
 * \ingroup Pions
 * 
 * \brief Class def header for a class SinglePions
 *
 * @author laube
 */

/** \addtogroup Pions

    @{*/

#ifndef LARLITE_SINGLEPIONS_H
#define LARLITE_SINGLEPIONS_H

#include "Analysis/ana_base.h"
#include "DataFormat/mcpart.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/mctrack.h"
#include "DataFormat/mcshower.h"
#include "LArUtil/Geometry.h"
#include "DataFormat/event_ass.h"
#include "DataFormat/track.h"
#include "DataFormat/vertex.h"
#include "DataFormat/pfpart.h"
#include <TH1.h>
#include <THStack.h>
#include "count.h"
#include <TH3.h>
namespace larlite {
  /**
     \class SinglePions
     User custom analysis class made by SHELL_USER_NAME
   */
  class SinglePions : public ana_base{
  
  public:

    /// Default constructor
    SinglePions(){ _name="SinglePions"; _fout=0;}

    /// Default destructor
    virtual ~SinglePions(){}

    /** IMPLEMENT in SinglePions.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in SinglePions.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in SinglePions.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

 
    int mcpart = 0;
    double PionMom = 0;
    double PionE = 0;
    TVector3 PionP;
    unsigned int PionTrackID = -99999;
   
    TH1D *PionLength 		= new TH1D("PionLength", "Length of primary Pion", 40, 0, 0);
    TH1D *PionEnergy    	= new TH1D("PionEnergy", "Energy of primary Pion", 40, 0, 0);
    TH1D *PionMomentum		= new TH1D("PionMomentum", "Momentum of primary Pion", 40, 0, 2);
    TH1D *DecayPionEnergy 	= new TH1D("DecayPionEnergy", "Energy of decaying Pion", 40, 0, 0);
    TH1D *DecayPionMom		= new TH1D("DecayPionMom", "Momentum of decaying Pion", 40, 0, 2);
    TH1D *PiInelasticPionEnergy = new TH1D("PiInelasticPionEnergy", "Energy of inelastically scattering Pion", 40, 0, 0 );
    TH1D *PiInelasticPionMom	= new TH1D("PiInelasticPionMom", "Momentum of inelastically scattering Pion", 40, 0, 2);
    TH1D *ExitPionEnergy	= new TH1D("ExitPionEnergy", "Energy of Pion exiting TPC without interacting", 40, 0, 0);
    TH1D *ExitPionMom		= new TH1D("ExitPionMom", "Momentum of Pion exiting TPC without interacting", 40, 0, 2);
    TH3D *EndPoint		= new TH3D("EndPoint", "Muon end point position", 300, -25, 275,260, -130, 130, 1200, 0, 1200); 
    TH1D *MinEndPointDist	= new TH1D("MinEndPointDist", "Minimum distance to any TPC Border", 100, 0, 0);

    THStack *InteractionEnergy 	= new THStack("InteractionEnergy", "Energy of single Pions by interaction type");
    THStack *InteractionMom	= new THStack("InteractionMom", "Momentum of single Pions by interaction type");
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
           //std::cout << pos.X() << ", " << pos.Y() << ", " << pos.Z() << std::endl;
	   disp -= pos;
           len += disp.Mag();
           disp = pos;
         }
       } 
       return len;
     }       

  double minimumValue(double dArray[]) {
    int iCurrMin = 0;
    for (int i =0; i< 6; i++) {
        if (dArray[i] < dArray[iCurrMin]) {
            iCurrMin = i;
        }
    }
    return dArray[iCurrMin];
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
