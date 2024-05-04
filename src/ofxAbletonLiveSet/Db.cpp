#include "Db.h"

OFX_ALS_BEGIN_NAMESPACE

Db::~Db(){

}

Db::Db(){

}


void Db::resetIndexes() {
	nextServedNoteIndex = 0;
	nextServedTrackIndex = 0;
}


//
bool Db::parseNotes( LiveSet& LS ){

	// ALS loaded ?
	if( !LS.isLoaded() ){
		ofLogNotice("OfxAbletonLiveSet::Db::parseNotes()", "before parsing events, make sure to parse() a live set.");
		return false;
	}
	
	// clean up
	LSNoteEvents.clear();
	
	// loop trough LS data
	for (std::size_t trackNb = 0; trackNb < LS.miditracks.size(); trackNb++){
		std::size_t nthNote=0;
		
		for(std::size_t clipNb=0; clipNb < LS.miditracks[trackNb].clips.size(); clipNb++){
			
			string clipName = LS.miditracks[trackNb].clips[clipNb].name;
			unsigned int clipColor = LS.miditracks[trackNb].clips[clipNb].color;
			std::size_t nthInClip = 0;
			
			for( vector<Note>::iterator it=LS.miditracks[trackNb].clips[clipNb].notes.begin(); it != LS.miditracks[trackNb].clips[clipNb].notes.end(); it++ ){
				
				LSNoteEvent noteEvent( clipName, clipColor, nthNote, nthInClip, trackNb, *it );
				
				LSNoteEvents.push_back(noteEvent);
				
				nthInClip ++;
			}
		}
	}
	sort(LSNoteEvents.begin(), LSNoteEvents.end(), sort_by_time<LSNoteEvent>);
	
	nextServedNoteIndex = 0;
	bNotesParsed = true;

	return true;
}

const LSNoteEvent* Db::getNextNote( float curTime ) {
	if(LSNoteEvents.size() < nextServedNoteIndex) return nullptr;

	for(std::size_t i=nextServedNoteIndex; i < LSNoteEvents.size(); i++){
		if(curTime >= LSNoteEvents[i].note.time){
			// remember
			nextServedNoteIndex = i + 1;
			return &LSNoteEvents[i];
		}
		// interrupt for loop ?
		if(LSNoteEvents[i].note.time > curTime) break;
	}
	return nullptr;
}

const LSTrackEvent* Db::getNextTrack( float curTime ){
	if(LSTrackEvents.size() < nextServedTrackIndex) return nullptr;
	
	for(std::size_t i=nextServedTrackIndex; i<LSTrackEvents.size(); i++){
		LSTrackEvent& trackData = LSTrackEvents[i];
		if(curTime >= trackData.audioClip.time){
			// remember
			nextServedTrackIndex=i+1;
			return &trackData;
		}
		// interrupt for loop ?
		if(LSTrackEvents[i].audioClip.time > curTime) break;
	}
	return nullptr;
}

//
bool Db::parseTracks( LiveSet& LS ){
	
	// ALS loaded ?
	if( !LS.isLoaded() ){
		ofLogNotice("OfxAbletonLiveSet::Db::parseTracks()", "before parsing events, make sure to parse() a live set.");
		return false;
	}
	
	// clean up
	LSTrackEvents.clear();
	
	// loop trough LS data
	for (std::size_t trackNb = 0; trackNb < LS.audiotracks.size(); trackNb++){
		
		for(std::size_t clipNb=0; clipNb < LS.audiotracks[trackNb].clips.size(); clipNb++){
				
			LSTrackEvent trackEvent( LS.audiotracks[trackNb].name, LS.audiotracks[trackNb].clips[clipNb], trackNb, clipNb );
			
			LSTrackEvents.push_back(trackEvent);
			
		}
	}
	sort(LSTrackEvents.begin(), LSTrackEvents.end(), sort_by_audio_clip_time<LSTrackEvent>);
	
	nextServedTrackIndex = 0;
	bTrackParsed = true;

	return true;
}

OFX_ALS_END_NAMESPACE
