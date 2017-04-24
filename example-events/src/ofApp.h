#include "ofMain.h"

#include "ofxAbletonLiveSet.h"

class ofApp : public ofBaseApp {
public:

	void setup();
	void noteEventListener( ofx::AbletonLiveSet::LSNoteEvent & noteEvent );
	void trackEventListener( ofx::AbletonLiveSet::LSTrackEvent & trackEvent );
	void metronomEventListener( ofx::AbletonLiveSet::LSMetronomEvent & metronomEvent );
	
private:
	// keep an instance of this alive to be notified of events
	ofxAbletonLiveSet::EventHandler eventHandler;
};
