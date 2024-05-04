#include "EventHandler.h"

// prototype seems to be needed for static ofEvents
ofEvent<const ofx::AbletonLiveSet::LSNoteEvent> ofx::AbletonLiveSet::EventHandler::noteEvent;
ofEvent<const ofx::AbletonLiveSet::LSMetronomEvent> ofx::AbletonLiveSet::EventHandler::metronomEvent;
ofEvent<const ofx::AbletonLiveSet::LSTrackEvent> ofx::AbletonLiveSet::EventHandler::trackEvent;

OFX_ALS_BEGIN_NAMESPACE

EventHandler::~EventHandler(){
#ifndef OFX_ALS_WITHOUT_POCO
	delete timer;
#else
	// Stop & Wait thread
	waitForThread();
#endif
}

EventHandler::EventHandler(){
#ifndef OFX_ALS_WITHOUT_POCO
	timer = new Timer(0, 10);
#endif
	LSNoteEvents.clear();
}

void EventHandler::startThreadedTimer(){

#ifndef OFX_ALS_WITHOUT_POCO

	// already started?
	if(stopWatch.elapsed()>0) return;
	
	// start timer
	// This timer checks every 10 milleseconds for new events to fire.
	// Used code from http://forum.openframeworks.cc/t/my-struggle-with-the-timing/2440 which seems to do the right job altough it remains not realtime.
	
	timer->start(TimerCallback<EventHandler>(*this, &EventHandler::threadedTimerTick ), Thread::PRIO_HIGHEST);
	stopWatch.start();

#else
	if(isThreadRunning()) return;

	startTime = ofGetElapsedTimef();
	startThread();

#endif
}


//template <class ListenerClass>
bool EventHandler::enableNoteEvents(  ){

	if( !bNotesParsed ) return false; // Need to parse first !
	if( LSNoteEvents.size() < 1 ) return true; // Exit early, no events to fire
	
	startThreadedTimer();
	
	resetIndexes();
	
	bNoteEvents = true;
	
	return true;
}

bool EventHandler::enableNoteEvents(ofx::AbletonLiveSet::LiveSet &LS){

	bool ret = parseNotes(LS);
	ret *= enableNoteEvents();
	return ret;
}


void EventHandler::fireNextNoteEvents(Time curTime){

	// stopWatch.elapsed() is in milliseconds
	// note.time is in seconds
	
	while( const LSNoteEvent* newNote = getNextNote(curTime) ){
		ofNotifyEvent( noteEvent, (const LSNoteEvent&)*newNote );
	}
}

bool EventHandler::enableTrackEvents(  ){

	if( !bTrackParsed ) return false; // Need to parse first !
	if( LSTrackEvents.size() < 1 ) return true; // Exit early, no events to fire
	
	startThreadedTimer();
	
	resetIndexes();
	
	bTrackEvents = true;
	
	return true;
}

bool EventHandler::enableTrackEvents(ofx::AbletonLiveSet::LiveSet &LS){
	bool ret = parseTracks(LS);
	ret *= enableTrackEvents();
	return ret;
}

	
void EventHandler::fireNextTrackEvents(Time curTime){

	// stopWatch.elapsed() is in milliseconds
	// Track.time is in seconds
	
	while( const LSNoteEvent* newNote = getNextNote(curTime/1000000) ){
		ofNotifyEvent( noteEvent, (const LSNoteEvent&)*newNote );
	}
}

//
bool EventHandler::enableMetronomEvents(){
	startThreadedTimer();
	
	nextMetronomEvent.clear();
	nextMetronomEvent.resize(LSMetronomEvents.size(), 0);
	
	bMetronomEvents = true;
	
	return true;
}

bool EventHandler::enableMetronomEvents(ofx::AbletonLiveSet::LiveSet &LS){
	
	return parseMetronomEvents(LS) && enableMetronomEvents();
}

bool EventHandler::parseMetronomEvents(ofx::AbletonLiveSet::LiveSet &LS){
	
	if( !LS.isLoaded() ){
		ofLogNotice("OfxAbletonLiveSet::EventHandler::parseMetronomEvents()", "before parsing events, make sure to parse() a live set.");
		return false;
	}
	
	LSMetronomEvents.clear();
	
	// loop trough LS data
	for (std::size_t trackNb = 0; trackNb < LS.miditracks.size(); trackNb++){
		LSMetronomEvent LSE;
		LSE.timeSignature = LS.miditracks[trackNb].timeSignature;
		LSE.trackNb = trackNb;
		LSE.bpm = LS.tempo.getGlobalTempo();
		LSMetronomEvents.push_back(LSE);
	}
	return true;
}


void EventHandler::fireNextMetronomEvents(Time curTime){
	// todo: the system can only handle 1 metronom for now...
	for(std::size_t i=0; i<LSMetronomEvents.size(); i++){
		
		if(curTime < nextMetronomEvent[i] ) continue;
			
		LSMetronomEvent LSE( LSMetronomEvents[i] );
		float oneBar = 60.0f/LSE.bpm;
		LSE.barTime = floor( (curTime)/oneBar );
		LSE.realTime = LSE.barTime * oneBar;
		LSE.isAccent = (LSE.barTime%LSE.timeSignature.numerator)==0;
		
		// Fixme : So... metronom events stack up infinitely as they get fired ?!
		nextMetronomEvent[i] = LSE.realTime+oneBar;
		
		ofNotifyEvent( ofx::AbletonLiveSet::EventHandler::metronomEvent, LSE );
	}

}

// the callback remains in the timer thread. Maybe need to lock mutex ?
#ifndef OFX_ALS_WITHOUT_POCO
void EventHandler::threadedTimerTick(Timer& _timer){
	Time time = stopWatch.elapsed()/1000000.0;
#else
void EventHandler::threadedTimerTick(Time time){
#endif

	// filter out unneccesary calls
	if(bNoteEvents){
		// no more notes ?
		if(LSNoteEvents.size() <= nextServedNoteIndex){
			bNoteEvents = false;
			return;
		}
		if( time >= LSNoteEvents[nextServedNoteIndex].note.time ){
			fireNextNoteEvents( time );
		}
	}
	if(bMetronomEvents){
		//if( elapsedSeconds >= nextMetronomEvent )
		fireNextMetronomEvents( time );
	}
	if(bTrackEvents){
		//if( elapsedSeconds >= nextMetronomEvent )
		fireNextTrackEvents( time );
	}
	
	return;
}

#ifdef OFX_ALS_WITHOUT_POCO
void EventHandler::threadedFunction(){
	while(isThreadRunning()){
		threadedTimerTick( ofGetElapsedTimef()-startTime );
		sleep(10); // fixme : find correct value for timer
	}
}
#endif

OFX_ALS_END_NAMESPACE
