/**
 * @file   OpDetWaveformMetaMaker_module.cc
 * @brief  Writes a collection of sbn::OpDetWaveformMeta from PMT waveforms.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   November 22, 2021
 */


// ICARUS libraries
<<<<<<< HEAD:icaruscode/Analysis/trigger/PMTcoverageInfoMaker_module.cc
#include "icaruscode/Analysis/trigger/Objects/PMTcoverageInfo.h"
||||||| parent of 77e62c26 (Renamed sbn::PMTcoverageInfo into sbn::OpDetWaveformMeta):icaruscode/PMT/PMTcoverageInfoMaker_module.cc
#include "icaruscode/IcarusObj/PMTcoverageInfo.h"
#include "icaruscode/PMT/Algorithms/PMTcoverageInfoUtils.h" // PMTcoverageInfoMaker
=======
#include "icaruscode/IcarusObj/OpDetWaveformMeta.h"
#include "icaruscode/PMT/Algorithms/OpDetWaveformMetaUtils.h" // OpDetWaveformMetaMaker
>>>>>>> 77e62c26 (Renamed sbn::PMTcoverageInfo into sbn::OpDetWaveformMeta):icaruscode/PMT/OpDetWaveformMetaMaker_module.cc

// LArSoft libraries
#include "lardata/DetectorInfoServices/DetectorClocksService.h"
#include "larcore/CoreUtils/ServiceUtil.h" // lar::providerFrom()
#include "lardataalg/DetectorInfo/DetectorTimings.h"
#include "lardataalg/DetectorInfo/DetectorTimingTypes.h" // electronics_time
#include "larcorealg/CoreUtils/enumerate.h"
#include "lardataobj/RawData/OpDetWaveform.h"

// framework libraries
#include "art/Framework/Core/SharedProducer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Persistency/Common/PtrMaker.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/Ptr.h"
#include "canvas/Utilities/InputTag.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "fhiclcpp/types/Atom.h"

// C/C++ standard libraries
#include <vector>
#include <string>
#include <memory> // std::make_unique()
#include <utility> // std::move()
#include <cassert>


//------------------------------------------------------------------------------
namespace icarus::trigger { class OpDetWaveformMetaMaker; }

/**
 * @brief Extracts and saves the time coverage of optical detector waveforms.
 * 
<<<<<<< HEAD:icaruscode/Analysis/trigger/PMTcoverageInfoMaker_module.cc
 * This module writes a list of `sim::BeamGateInfo` based on the time associated
 * to a selection of reconstructed tracks.
||||||| parent of 77e62c26 (Renamed sbn::PMTcoverageInfo into sbn::OpDetWaveformMeta):icaruscode/PMT/PMTcoverageInfoMaker_module.cc
 * This module writes a list of `sbn::PMTcoverageInfo` objects matching the
 * information of each optical detector waveform.
=======
 * This module writes a list of `sbn::OpDetWaveformMeta` objects matching the
 * information of each optical detector waveform.
>>>>>>> 77e62c26 (Renamed sbn::PMTcoverageInfo into sbn::OpDetWaveformMeta):icaruscode/PMT/OpDetWaveformMetaMaker_module.cc
 * 
 * It may be used as input to modules which require to operate on beam gates,
 * to select time(s) around the reconstructed (and selected) tracks.
 * 
 * 
 * Input data products
 * ====================
 * 
 * This module acts on a _selection_ of tracks, which implies that the input is
 * a set of _pointers_ to tracks rather than to an actual track collection.
 * For each track, an associated time is required.
 * 
 * * `std::vector<raw::OpDetWaveform>` (tag from `Waveforms`):
 *   all optical detector waveforms to extract the information from
 * 
 * 
 * Output data products
 * =====================
 *
 * * `std::vector<sbn::OpDetWaveformMeta>`: a collection parallel to the input
 *   one (from data product configured by `Waveforms`) with the summary
 *   information on each of them; also an explicit courtesy association
 *   `art::Assns<sbn::OpDetWaveformMeta, raw::OpDetWaveform>` for uses where
 *   order is not preserved.
 *   The times in these objects are on the same scale as the ones in the source
 *   data product, which is expected to be
 *   @ref DetectorClocksElectronicsStartTime "electronics time scale [us]".
 * 
 * 
 * Configuration parameters
 * =========================
 * 
 * A terse online description of the parameters is printed by running
 * `lar --print-description OpDetWaveformMetaMaker`.
 * 
 * * `Waveforms` (input tag, mandatory): the list of optical detector waveforms
 *   to be processed.
 * * `LogCategory` (string, default: `OpDetWaveformMetaMaker`): name of the
 *     output stream category for console messages (managed by MessageFacility
 *     library).
 * 
 * 
 */
class icarus::trigger::OpDetWaveformMetaMaker: public art::SharedProducer {
  
    public:
  
  // --- BEGIN Configuration ---------------------------------------------------
  struct Config {
    
    using Name = fhicl::Name;
    using Comment = fhicl::Comment;
    
    
    fhicl::Atom<art::InputTag> Waveforms {
      Name("Waveforms"),
      Comment("tag of input optical detector waveforms")
      // mandatory
      };
    
    fhicl::Atom<std::string> LogCategory {
      Name("LogCategory"),
      Comment("name of the category used for the output"),
      "OpDetWaveformMetaMaker" // default
      };
    
  }; // struct Config
  
  using Parameters = art::SharedProducer::Table<Config>;
  
  // --- END Configuration -----------------------------------------------------
  
  
  // --- BEGIN Constructors ----------------------------------------------------
  
  explicit OpDetWaveformMetaMaker
    (Parameters const& config, art::ProcessingFrame const&);
  
  // --- END Constructors ------------------------------------------------------
  
  
  // --- BEGIN Framework hooks -------------------------------------------------
  
  /// Fills the plots. Also extracts the information to fill them with.
  virtual void produce(art::Event& event, art::ProcessingFrame const&) override;
  
  // --- END Framework hooks ---------------------------------------------------
  
  
    private:
  
  // --- BEGIN Configuration variables -----------------------------------------
  
  art::InputTag const fWaveformTag; ///< Input waveforms.
  
  /// Message facility stream category for output.
  std::string const fLogCategory;
  
  // --- END Configuration variables -------------------------------------------
  
  
}; // class icarus::trigger::OpDetWaveformMetaMaker


//------------------------------------------------------------------------------
//--- Implementation
//------------------------------------------------------------------------------
namespace {
  
  template <typename T>
  std::unique_ptr<T> moveToUniquePtr(T& data)
    { return std::make_unique<T>(std::move(data)); }

} // local namespace


//------------------------------------------------------------------------------
icarus::trigger::OpDetWaveformMetaMaker::OpDetWaveformMetaMaker
  (Parameters const& config, art::ProcessingFrame const&)
  : art::SharedProducer(config)
  // configuration
  , fWaveformTag(config().Waveforms())
  , fLogCategory(config().LogCategory())
{
  
  async<art::InEvent>();
  
  //
  // output data declaration
  //
  produces<std::vector<sbn::OpDetWaveformMeta>>();
  produces<art::Assns<sbn::OpDetWaveformMeta, raw::OpDetWaveform>>();
  
  //
  // configuration report (short)
  //
  
  mf::LogInfo{ fLogCategory }
    << "Configuration:"
    << "\n - input waveforms: '" << fWaveformTag.encode() << '\''
    ;
  
} // icarus::trigger::OpDetWaveformMetaMaker::OpDetWaveformMetaMaker()


//------------------------------------------------------------------------------
void icarus::trigger::OpDetWaveformMetaMaker::produce
  (art::Event& event, art::ProcessingFrame const&)
{
  
  using detinfo::timescales::electronics_time;
  
  //
  // get the timing information for this event
  //
  detinfo::DetectorTimings const detTimings = detinfo::makeDetectorTimings
    (art::ServiceHandle<detinfo::DetectorClocksService const>()->DataFor(event))
    ;
  
  auto const opDetTickPeriod = detTimings.OpticalClockPeriod();
  electronics_time const triggerTime = detTimings.TriggerTime();
  electronics_time const beamGateTime = detTimings.BeamGateTime();
  
  //
  // fetch input
  //
  auto const& waveformHandle
    = event.getValidHandle<std::vector<raw::OpDetWaveform>>(fWaveformTag);

  mf::LogDebug(fLogCategory)
    << "Event " << event.id() << " has beam gate starting at " << beamGateTime
    << " and trigger at " << triggerTime << "."
    << "\nNow extracting information from " << waveformHandle->size()
      << " waveforms."
    ;

  //
  // create the content
  //
<<<<<<< HEAD:icaruscode/Analysis/trigger/PMTcoverageInfoMaker_module.cc
  std::vector<sbn::PMTcoverageInfo> PMTinfo;
  art::Assns<sbn::PMTcoverageInfo, raw::OpDetWaveform> infoToWaveform;
||||||| parent of 77e62c26 (Renamed sbn::PMTcoverageInfo into sbn::OpDetWaveformMeta):icaruscode/PMT/PMTcoverageInfoMaker_module.cc
  sbn::PMTcoverageInfoMaker makePMTcoverageInfo{ detTimings };
  
  std::vector<sbn::PMTcoverageInfo> PMTinfo;
  art::Assns<sbn::PMTcoverageInfo, raw::OpDetWaveform> infoToWaveform;
=======
  sbn::OpDetWaveformMetaMaker makeOpDetWaveformMeta{ detTimings };
  
  std::vector<sbn::OpDetWaveformMeta> PMTinfo;
  art::Assns<sbn::OpDetWaveformMeta, raw::OpDetWaveform> infoToWaveform;
>>>>>>> 77e62c26 (Renamed sbn::PMTcoverageInfo into sbn::OpDetWaveformMeta):icaruscode/PMT/OpDetWaveformMetaMaker_module.cc
  
  art::PtrMaker<sbn::OpDetWaveformMeta> const makeInfoPtr { event };
  art::PtrMaker<raw::OpDetWaveform> const makeWaveformPtr
    { event, waveformHandle.id() };
  
  for (auto const& [ iWaveform, waveform ]: util::enumerate(*waveformHandle)) {
    assert(iWaveform == PMTinfo.size());
    
<<<<<<< HEAD:icaruscode/Analysis/trigger/PMTcoverageInfoMaker_module.cc
    raw::Channel_t const channel = waveform.ChannelNumber();
    electronics_time const startTime { waveform.TimeStamp() };
    electronics_time const endTime
      = startTime + waveform.Waveform().size() * opDetTickPeriod;
    
    auto const isInWaveform = [startTime,endTime](electronics_time t)
      { return (t >= startTime) && (t < endTime); };
    
    bool const containsTrigger = isInWaveform(triggerTime);
    bool const containsBeamGateStart = isInWaveform(beamGateTime);
||||||| parent of 77e62c26 (Renamed sbn::PMTcoverageInfo into sbn::OpDetWaveformMeta):icaruscode/PMT/PMTcoverageInfoMaker_module.cc
    PMTinfo.push_back(makePMTcoverageInfo(waveform));
=======
    PMTinfo.push_back(makeOpDetWaveformMeta(waveform));
>>>>>>> 77e62c26 (Renamed sbn::PMTcoverageInfo into sbn::OpDetWaveformMeta):icaruscode/PMT/OpDetWaveformMetaMaker_module.cc
    
    {
      mf::LogTrace log{ fLogCategory };
      log << "Coverage for waveform #" << iWaveform
        << " on channel " << channel << ": " << startTime << " -- " << endTime;
      if (containsTrigger) log << "; includes trigger";
      if (containsBeamGateStart) log << "; includes beam gate start";
    }
    
    sbn::PMTcoverageInfo info {
        waveform.ChannelNumber()  // channel
      , startTime.value()         // startTime
      , endTime.value()           // endTime
      /* the following are left default:
      // setFlags
      // definedFlags
      */
      };
    
    info.set(sbn::PMTcoverageInfo::bits::WithTrigger, containsTrigger);
    info.set(sbn::PMTcoverageInfo::bits::WithBeamGate, containsBeamGateStart);
    
    PMTinfo.push_back(std::move(info));
    infoToWaveform.addSingle
      (makeInfoPtr(iWaveform), makeWaveformPtr(iWaveform));
    
  } // for
  
  //
  // store output
  //
  event.put(moveToUniquePtr(PMTinfo));
  event.put(moveToUniquePtr(infoToWaveform));
  
} // icarus::trigger::OpDetWaveformMetaMaker::produce()


//------------------------------------------------------------------------------
DEFINE_ART_MODULE(icarus::trigger::OpDetWaveformMetaMaker)


//------------------------------------------------------------------------------
