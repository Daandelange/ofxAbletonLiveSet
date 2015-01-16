#pragma once

#include "Poco/Stopwatch.h"
#include "Poco/Thread.h"
#include "Poco/Timestamp.h"
#include "Poco/Timer.h"

#include "Constants.h"
#include "Tempo.h"
#include "Model.h"

OFX_ALS_BEGIN_NAMESPACE

using Poco::Stopwatch;
using Poco::Thread;
using Poco::Timer;
using Poco::TimerCallback;

class EventHandler {
public:
	~EventHandler();
	EventHandler();
	
	bool enableNoteEvents( );
	bool enableNoteEvents( LiveSet& LS );
	bool parseNoteEvents( LiveSet& LS );
	
	bool enableMetronomEvents( );
	bool enableMetronomEvents( LiveSet& LS );
	bool parseMetronomEvents( LiveSet& LS );
	
	void threadedTimerTick(Timer& timer);
	
	// OF event listener / notifier
	static ofEvent<LSNoteEvent> noteEvent;
	static ofEvent<LSMetronomEvent> metronomEvent;
	
private:
	
	bool bNoteEvents = false;
	bool bMetronomEvents = false;
	
	int currentNoteEventIndex = 0;
	vector<Poco::Timestamp::TimeDiff> nextMetronomEvent;
	
	vector<LSNoteEvent> LSNoteEvents;
	vector<LSMetronomEvent> LSMetronomEvents;
	
	Stopwatch stopWatch;
	Timer * timer;
	
	void startThreadedTimer();
	
	void fireNextNoteEvents( Poco::Timestamp::TimeDiff curTime );
	void fireNextMetronomEvents( Poco::Timestamp::TimeDiff curTime );
	
	template <typename T>
	static bool sort_by_time(const T& v0, const T& v1) { return v0.note.time < v1.note.time; }
};

OFX_ALS_END_NAMESPACE
