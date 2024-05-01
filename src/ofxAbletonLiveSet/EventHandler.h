#pragma once

#include "Constants.h"
#include "Tempo.h"
#include "Model.h"

#ifndef OFX_ALS_WITHOUT_POCO
#include "Poco/Stopwatch.h"
#include "Poco/Thread.h"
#include "Poco/Timestamp.h"
#include "Poco/Timer.h"
#endif

OFX_ALS_BEGIN_NAMESPACE

#ifndef OFX_ALS_WITHOUT_POCO
using Poco::Stopwatch;
using Poco::Thread;
using Poco::Timer;
using Poco::TimerCallback;
#endif

class EventHandler {
public:
	~EventHandler();
	EventHandler();
	
#ifndef OFX_ALS_WITHOUT_POCO
	bool enableNoteEvents( );
	bool enableNoteEvents( LiveSet& LS );
#endif
	bool parseNoteEvents( LiveSet& LS );
	
#ifndef OFX_ALS_WITHOUT_POCO
	bool enableTrackEvents( );
	bool enableTrackEvents( LiveSet& LS );
#endif
	bool parseTrackEvents( LiveSet& LS );

#ifndef OFX_ALS_WITHOUT_POCO
	bool enableMetronomEvents( );
	bool enableMetronomEvents( LiveSet& LS );
#endif
	bool parseMetronomEvents( LiveSet& LS );

#ifndef OFX_ALS_WITHOUT_POCO
	void threadedTimerTick(Timer& timer);
#endif

	// OF event listener / notifier
	static ofEvent<LSNoteEvent> noteEvent;
	static ofEvent<LSTrackEvent> trackEvent;
	static ofEvent<LSMetronomEvent> metronomEvent;
	
private:
	
	bool bNoteEvents = false;
	bool bTrackEvents = false;
	bool bMetronomEvents = false;
	
	int currentNoteEventIndex = 0;
	int currentTrackEventIndex = 0;
#ifndef OFX_ALS_WITHOUT_POCO
	vector<Poco::Timestamp::TimeDiff> nextMetronomEvent;
#endif

	vector<LSNoteEvent> LSNoteEvents;
	vector<LSTrackEvent> LSTrackEvents;
	vector<LSMetronomEvent> LSMetronomEvents;

#ifndef OFX_ALS_WITHOUT_POCO
	Stopwatch stopWatch;
	Timer * timer;
#endif
	
	void startThreadedTimer();

#ifndef OFX_ALS_WITHOUT_POCO
	void fireNextNoteEvents( Poco::Timestamp::TimeDiff curTime );
	void fireNextTrackEvents( Poco::Timestamp::TimeDiff curTime );
	void fireNextMetronomEvents( Poco::Timestamp::TimeDiff curTime );
#endif
	
	template <typename T>
	static bool sort_by_time(const T& v0, const T& v1) { return v0.note.time < v1.note.time; }
	
	template <typename T>
	static bool sort_by_audio_clip_time(const T& v0, const T& v1) { return v0.audioClip.time < v1.audioClip.time; }
};

OFX_ALS_END_NAMESPACE
