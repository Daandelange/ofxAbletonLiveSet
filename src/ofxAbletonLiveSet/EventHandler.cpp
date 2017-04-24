#include "EventHandler.h"

// prototype seems to be needed for static ofEvents
ofEvent<ofx::AbletonLiveSet::LSNoteEvent> ofx::AbletonLiveSet::EventHandler::noteEvent;
ofEvent<ofx::AbletonLiveSet::LSMetronomEvent> ofx::AbletonLiveSet::EventHandler::metronomEvent;
ofEvent<ofx::AbletonLiveSet::LSTrackEvent> ofx::AbletonLiveSet::EventHandler::trackEvent;

OFX_ALS_BEGIN_NAMESPACE

EventHandler::~EventHandler(){
	delete timer;
}

EventHandler::EventHandler(){
	timer = new Timer(0, 10);
	LSNoteEvents.clear();
}

void EventHandler::startThreadedTimer(){
	// already started?
	if(stopWatch.elapsed()>0) return;
	
	// start timer
	// This timer checks every 10 milleseconds for new events to fire.
	// Used code from http://forum.openframeworks.cc/t/my-struggle-with-the-timing/2440 which seems to do the right job altough it remains not realtime.
	
	timer->start(TimerCallback<EventHandler>(*this, &EventHandler::threadedTimerTick ), Thread::PRIO_HIGHEST);
	stopWatch.start();
}

//template <class ListenerClass>
bool EventHandler::enableNoteEvents(  ){
	if( LSNoteEvents.size() < 1 ) return false;
	
	startThreadedTimer();
	
	currentNoteEventIndex = 0;
	
	bNoteEvents = true;
	
	return true;
}

bool EventHandler::enableNoteEvents(ofx::AbletonLiveSet::LiveSet &LS){
	parseNoteEvents(LS);
	enableNoteEvents();
}



void EventHandler::fireNextNoteEvents(Poco::Timestamp::TimeDiff curTime){
	if(LSNoteEvents.size() < currentNoteEventIndex) return;
	
	// stopWatch.elapsed() is in milliseconds
	// note.time is in seconds
	
	for(int i=currentNoteEventIndex; i<LSNoteEvents.size(); i++){
		if(curTime >= LSNoteEvents[i].note.time*1000000){
			// fire the event!
			ofNotifyEvent( noteEvent, LSNoteEvents[i] );
			
			// remember
			currentNoteEventIndex=i+1;
		}
		// interrupt for loop ?
		if(LSNoteEvents[i].note.time*1000000 > curTime) break;
	}
}
//
bool EventHandler::parseNoteEvents( LiveSet& LS ){
	
	// ALS loaded ?
	if( !LS.isLoaded() ){
		ofLogNotice("OfxAbletonLiveSet::EventHandler::parseNoteEvents()", "before parsing events, make sure to parse() a live set.");
		return false;
	}
	
	// clean up
	LSNoteEvents.clear();
	
	// loop trough LS data
	for (int trackNb = 0; trackNb < LS.miditracks.size(); trackNb++){
		int nthNote=0;
		
		for(int clipNb=0; clipNb < LS.miditracks[trackNb].clips.size(); clipNb++){
			
			string clipName = LS.miditracks[trackNb].clips[clipNb].name;
			int clipColor = LS.miditracks[trackNb].clips[clipNb].color;
			int nthInClip = 0;
			
			for( vector<Note>::iterator it=LS.miditracks[trackNb].clips[clipNb].notes.begin(); it != LS.miditracks[trackNb].clips[clipNb].notes.end(); it++ ){
				
				LSNoteEvent noteEvent( clipName, clipColor, nthNote, nthInClip, trackNb, *it );
				
				LSNoteEvents.push_back(noteEvent);
				
				nthInClip ++;
			}
		}
	}
	std:sort(LSNoteEvents.begin(), LSNoteEvents.end(), sort_by_time<LSNoteEvent>);
	
	return true;
}

bool EventHandler::enableTrackEvents(  ){
	if( LSTrackEvents.size() < 1 ) return false;
	
	startThreadedTimer();
	
	currentTrackEventIndex = 0;
	
	bTrackEvents = true;
	
	return true;
}

bool EventHandler::enableTrackEvents(ofx::AbletonLiveSet::LiveSet &LS){
	parseTrackEvents(LS);
	enableTrackEvents();
}



void EventHandler::fireNextTrackEvents(Poco::Timestamp::TimeDiff curTime){
	if(LSTrackEvents.size() < currentTrackEventIndex) return;
	
	// stopWatch.elapsed() is in milliseconds
	// Track.time is in seconds
	
	for(int i=currentTrackEventIndex; i<LSTrackEvents.size(); i++){
		if(curTime >= LSTrackEvents[i].audioClip.time*1000000){
			// fire the event!
			ofNotifyEvent( trackEvent, LSTrackEvents[i] );
			
			// remember
			currentTrackEventIndex=i+1;
		}
		// interrupt for loop ?
		if(LSTrackEvents[i].audioClip.time*1000000 > curTime) break;
	}
}

//
bool EventHandler::parseTrackEvents( LiveSet& LS ){
	
	// ALS loaded ?
	if( !LS.isLoaded() ){
		ofLogNotice("OfxAbletonLiveSet::EventHandler::parseTrackEvents()", "before parsing events, make sure to parse() a live set.");
		return false;
	}
	
	// clean up
	LSTrackEvents.clear();
	
	// loop trough LS data
	for (int trackNb = 0; trackNb < LS.audiotracks.size(); trackNb++){
		
		for(int clipNb=0; clipNb < LS.audiotracks[trackNb].clips.size(); clipNb++){
				
			LSTrackEvent trackEvent( LS.audiotracks[trackNb].name, LS.audiotracks[trackNb].clips[clipNb], trackNb, clipNb );
			
			LSTrackEvents.push_back(trackEvent);
			
			clipNb ++;
		}
	}
	std:sort(LSTrackEvents.begin(), LSTrackEvents.end(), sort_by_audio_clip_time<LSTrackEvent>);
	
	return true;
}

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
	for (int trackNb = 0; trackNb < LS.miditracks.size(); trackNb++){
		LSMetronomEvent LSE;
		LSE.timeSignature = LS.miditracks[trackNb].timeSignature;
		LSE.trackNb = trackNb;
		LSE.bpm = LS.tempo.getGlobalTempo();
		LSMetronomEvents.push_back(LSE);
	}
	return true;
}

void EventHandler::fireNextMetronomEvents(Poco::Timestamp::TimeDiff curTime){
	
	// todo: the system can only handle 1 metronom for now...
	for(int i=0; i<LSMetronomEvents.size(); i++){
		
		if(curTime < nextMetronomEvent[i] ) continue;
			
		LSMetronomEvent LSE( LSMetronomEvents[i] );
		float oneBar = 60.0f/LSE.bpm;
		LSE.barTime = floor( (curTime*1.0f/1000000)/oneBar );
		LSE.realTime = LSE.barTime * oneBar;
		LSE.isAccent = (LSE.barTime%LSE.timeSignature.numerator)==0;
		
		nextMetronomEvent[i] = LSE.realTime*1000000+oneBar*1000000;
		
		ofNotifyEvent( ofx::AbletonLiveSet::EventHandler::metronomEvent, LSE );
	}

}

// the callback remains in the timer thread. Maybe need to lock mutex ?
void EventHandler::threadedTimerTick(Timer& timer){

	// filter out unneccesary calls
	if(bNoteEvents){
		// no more notes ?
		if(LSNoteEvents.size()<=currentNoteEventIndex){
			bNoteEvents = false;
			return;
		}
		
		if( stopWatch.elapsed() >= LSNoteEvents[currentNoteEventIndex].note.time*1000000 ) fireNextNoteEvents( stopWatch.elapsed() );
	}
	if(bMetronomEvents){
		//if( stopWatch.elapsed() >= nextMetronomEvent )
		fireNextMetronomEvents( stopWatch.elapsed() );
	}
	if(bTrackEvents){
		//if( stopWatch.elapsed() >= nextMetronomEvent )
		fireNextTrackEvents( stopWatch.elapsed() );
	}
	
	return;
}

OFX_ALS_END_NAMESPACE
