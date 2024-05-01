#include "ofApp.h"

void ofApp::setup(){
	ofxAbletonLiveSet::LiveSet LS;
	ofxAbletonLiveSet::Parser parser(LS);
	
	if( !parser.open("mappingvendome.xml") ){
		cout << "Could not parse the live set..." << endl;
		ofExit();
		return;
	}
		
	// print global variables
	cout << "Global ALS variables" << endl;
	cout << "Tempo: " << (LS.tempo.getGlobalTempo() ) << endl;
	cout << "=========" << endl << endl;
		
	// track info
	for (int i = 0; i < LS.miditracks.size(); i++){
		const ofxAbletonLiveSet::MidiTrack &T = LS.miditracks.at(i);
		cout << "Track Name: " << T.name << endl;
		
		for (int j = 0; j < T.clips.size(); j++){
			const ofxAbletonLiveSet::MidiClip& m = T.clips.at(j);
			cout << "Clip Name: " << m.name << endl;
			cout << "Clip Start Time: " << m.time << endl;
			cout << "Clip Duration: " << m.duration << endl;
			
			for (int k = 0; k < m.notes.size(); k++){
				cout << "Note Start Time: " << m.notes[k].time << endl;
			}
			
			for (int k = 0; k < m.envelopes.size(); k++){
				cout << "Envelope ID: " << m.envelopes[k].id << endl;
			}
		}
		
		cout << "===" << endl << endl;
	}
	
	
	// show locators
	for (int i = 0; i < LS.locators.size(); i++)
	{
		const ofxAbletonLiveSet::Locator &L = LS.locators.at(i);
		cout << "Locator Name: " << L.name << endl;
		cout << "Locator Time: " << L.time << endl;
	}

	// AudioTracks
	for (int i = 0; i < LS.audiotracks.size(); i++)
	{
		const ofxAbletonLiveSet::AudioTrack &T = LS.audiotracks.at(i);
		cout << "AudioTrack Name: " << T.name << endl;

		for (int j = 0; j < T.clips.size(); j++)
		{
			const ofxAbletonLiveSet::AudioClip& m = T.clips.at(j);
			cout << "AudioClip Name: " << m.name << " (Time: " << m.time << " ->\t" << m.duration << ")" << endl;
		}

		cout << "===" << endl << endl;
	}
	
	ofExit();
}
