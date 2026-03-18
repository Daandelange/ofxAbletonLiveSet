#pragma once

// Utility to parse, store and traverse ALS data
// ALS data events sequencer
// ALS data scene manager

#include "Constants.h"
#include "Tempo.h"
#include "Model.h"

OFX_ALS_BEGIN_NAMESPACE

class Db {
public:
	~Db();
	Db();
	
	bool parseNotes( const LiveSet& LS );
	bool parseTracks( const LiveSet& LS );
	bool parseMetronoms( const LiveSet& LS );

	void resetIndexes();
	
	// To be used in a while loop until no more are left (returns nullptr).
	const LSNoteEvent* getNextNote( float curTime );
	const LSTrackEvent* getNextTrack( float curTime );

protected:
	
	bool bNotesParsed = false;
	bool bTrackParsed = false;
	
	std::size_t nextServedNoteIndex = 0;
	std::size_t nextServedTrackIndex = 0;

	// Parsed data, sorted by time
	vector<LSNoteEvent> LSNoteEvents;
	vector<LSTrackEvent> LSTrackEvents;
	
	template <typename T>
	static bool sort_by_time(const T& v0, const T& v1) { return v0.note.time < v1.note.time; }
	
	template <typename T>
	static bool sort_by_audio_clip_time(const T& v0, const T& v1) { return v0.audioClip.time < v1.audioClip.time; }
};

OFX_ALS_END_NAMESPACE
