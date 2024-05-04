#pragma once

#include "Constants.h"
#include "Tempo.h"
#include "Model.h"

OFX_ALS_BEGIN_NAMESPACE

class Db {
public:
	~Db();
	Db();
	
	bool parseNotes( LiveSet& LS );
	bool parseTracks( LiveSet& LS );
	bool parseMetronoms( LiveSet& LS );

	void resetIndexes();
	
protected:
	
	bool bNotesParsed = false;
	bool bTrackParsed = false;
	
	std::size_t nextServedNoteIndex = 0;
	std::size_t nextServedTrackIndex = 0;

	// Parsed data, sorted by time
	vector<LSNoteEvent> LSNoteEvents;
	vector<LSTrackEvent> LSTrackEvents;

	const LSNoteEvent* getNextNote( float curTime );
	const LSTrackEvent* getNextTrack( float curTime );
	
	template <typename T>
	static bool sort_by_time(const T& v0, const T& v1) { return v0.note.time < v1.note.time; }
	
	template <typename T>
	static bool sort_by_audio_clip_time(const T& v0, const T& v1) { return v0.audioClip.time < v1.audioClip.time; }
};

OFX_ALS_END_NAMESPACE
