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
#else
#include <chrono>
#endif

OFX_ALS_BEGIN_NAMESPACE

#ifndef OFX_ALS_WITHOUT_POCO
using Poco::Stopwatch;
using Poco::Thread;
using Poco::Timer;
using Poco::TimerCallback;
#endif

class EventHandler : protected ofx::AbletonLiveSet::Db
#ifdef OFX_ALS_WITHOUT_POCO
		, protected ofThread
#endif
{
public:
	~EventHandler();
	EventHandler();
	
	bool enableNoteEvents( );
	bool enableNoteEvents( LiveSet& LS );
	
	bool enableTrackEvents( );
	bool enableTrackEvents( LiveSet& LS );

	bool enableMetronomEvents( );
	bool enableMetronomEvents( LiveSet& LS );
	bool parseMetronomEvents( LiveSet& LS );

#ifndef OFX_ALS_WITHOUT_POCO
	void threadedTimerTick(Timer& _timer);
#else
	void threadedTimerTick(Time timer);
#endif

	// OF event listener / notifier
	static ofEvent<const LSNoteEvent> noteEvent;
	static ofEvent<const LSTrackEvent> trackEvent;
	static ofEvent<const LSMetronomEvent> metronomEvent;
	
private:
	
	bool bNoteEvents = false;
	bool bTrackEvents = false;
	bool bMetronomEvents = false;

//#ifndef OFX_ALS_WITHOUT_POCO
//	vector<Poco::Timestamp::TimeDiff> nextMetronomEvent;
//#else
	vector<Time> nextMetronomEvent;
//#endif

	vector<LSMetronomEvent> LSMetronomEvents;

#ifndef OFX_ALS_WITHOUT_POCO
	Stopwatch stopWatch;
	Timer * timer;
#else
	virtual void threadedFunction() override;
	float startTime; // /!\ Threaded variable !
#endif
	
	void startThreadedTimer();

	void fireNextNoteEvents( Time curTime );
	void fireNextTrackEvents( Time curTime );
	void fireNextMetronomEvents( Time curTime );
	
	
};

OFX_ALS_END_NAMESPACE
