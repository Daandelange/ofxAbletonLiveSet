#pragma once

#include "Constants.h"
#include "Tempo.h"
#include "Model.h"
#include "Db.h"

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

class EventHandler : ofx::AbletonLiveSet::Db {
public:
	~EventHandler();
	EventHandler();
	
#ifndef OFX_ALS_WITHOUT_POCO
	bool enableNoteEvents( );
	bool enableNoteEvents( LiveSet& LS );
#endif
	
#ifndef OFX_ALS_WITHOUT_POCO
	bool enableTrackEvents( );
	bool enableTrackEvents( LiveSet& LS );
#endif

#ifndef OFX_ALS_WITHOUT_POCO
	bool enableMetronomEvents( );
	bool enableMetronomEvents( LiveSet& LS );
#endif
	bool parseMetronomEvents( LiveSet& LS );

#ifndef OFX_ALS_WITHOUT_POCO
	void threadedTimerTick(Timer& timer);
#endif

	// OF event listener / notifier
	static ofEvent<const LSNoteEvent> noteEvent;
	static ofEvent<const LSTrackEvent> trackEvent;
	static ofEvent<const LSMetronomEvent> metronomEvent;
	
private:
	
	bool bNoteEvents = false;
	bool bTrackEvents = false;
	bool bMetronomEvents = false;

#ifndef OFX_ALS_WITHOUT_POCO
	vector<Poco::Timestamp::TimeDiff> nextMetronomEvent;
#else
	vector<float> nextMetronomEvent;
#endif

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
	
	
};

OFX_ALS_END_NAMESPACE
