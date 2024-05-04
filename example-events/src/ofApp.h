#include "ofMain.h"

#include "ofxAbletonLiveSet.h"

class ofApp : public ofBaseApp {
public:

	void setup();
	void noteEventListener( const ofx::AbletonLiveSet::LSNoteEvent & noteEvent );
	void trackEventListener( const ofx::AbletonLiveSet::LSTrackEvent & trackEvent );
	void metronomEventListener( const ofx::AbletonLiveSet::LSMetronomEvent & metronomEvent );
	
private:
	// keep an instance of this alive to be notified of events
	ofxAbletonLiveSet::EventHandler eventHandler;
};
