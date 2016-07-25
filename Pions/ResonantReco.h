/**
 * \file ResonantReco.h
 *
 * \ingroup Pions
 * 
 * \brief Class def header for a class ResonantReco
 *
 * @author laube
 */

/** \addtogroup Pions

    @{*/

#ifndef LARLITE_RESONANTRECO_H
#define LARLITE_RESONANTRECO_H

#include "Analysis/ana_base.h"
#include "Analysis/ana_base.h"
#include "DataFormat/mcpart.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/mctrack.h"
#include "DataFormat/mcshower.h"
#include "LArUtil/Geometry.h"
#include <TH1.h>

namespace larlite {
  /**
     \class ResonantReco
     User custom analysis class made by SHELL_USER_NAME
   */
  class ResonantReco : public ana_base{
  
  public:

    /// Default constructor
    ResonantReco(){ _name="ResonantReco"; _fout=0;}

    /// Default destructor
    virtual ~ResonantReco(){}

    /** IMPLEMENT in ResonantReco.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in ResonantReco.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in ResonantReco.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

      int all_events = 0;

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
