#include "ofApp.h"

void ofApp::setup(){
	ofxAbletonLiveSet::LiveSet LS;
	ofxAbletonLiveSet::Parser parser(LS);
	
	// Put your filename here ! (the provided one doesn't demonstrate all features!)
	// It can open either XML or ALS formats
#define FILENAME "Test.als"

	if( !parser.open(FILENAME) ){
		cout << "Could not parse the live set file :" << ofToDataPath(FILENAME) << endl;
		ofExit();
		return;
	}
		
	// print global variables
	cout << "Global ALS variables" << endl;
	cout << "Tempo: " << (LS.tempo.getGlobalTempo() ) << endl;
	cout << "=========" << endl << endl;
		
	// track info
	cout << "=========" << endl << "Midi Tracks (" << LS.miditracks.size() << ")" << endl << "- - - -" << endl;
	for (unsigned int i = 0; i < LS.miditracks.size(); i++){
		const ofxAbletonLiveSet::MidiTrack &T = LS.miditracks.at(i);
		cout << "Track Name: `" << T.name << "` (" << T.clips.size() << " clips)" << endl;
		
		for (unsigned int j = 0; j < T.clips.size(); j++){
			const ofxAbletonLiveSet::MidiClip& m = T.clips.at(j);
			cout << "Clip Name: `" << m.name << "`\t@" << m.time << " --> " << m.duration << "\t(" << m.notes.size() << " notes, " << m.envelopes.size() << " envelopes) \t Annotation=`" << m.annotation << "`."  << endl;
			
			if(false) for (unsigned int k = 0; k < m.notes.size(); k++){
				auto& mNote = m.notes[k];
				cout << "Note #" << mNote.key << "\t@" << mNote.time << " --> " << mNote.time+mNote.duration << endl;
			}
			
			for (unsigned int k = 0; k < m.envelopes.size(); k++){
				cout << "Envelope ID: " << m.envelopes[k].id << endl;
			}
		}
		
		cout << "- - - -" << endl;
	}

	// show locators
	cout << endl << "=========" << endl << "Locators (" << LS.locators.size() << ")" << endl << "- - - -" << endl;
	for (unsigned int i = 0; i < LS.locators.size(); i++)
	{
		const ofxAbletonLiveSet::Locator &L = LS.locators.at(i);
		cout << "Locator Name: " << L.name << endl;
		cout << "Locator Time: " << L.time << endl;
	}

	// AudioTracks
	cout << endl << "=========" << endl << "Audio Tracks (" << LS.audiotracks.size() << ")" << endl << "- - - -" << endl;
	for (unsigned int i = 0; i < LS.audiotracks.size(); i++)
	{
		const ofxAbletonLiveSet::AudioTrack &T = LS.audiotracks.at(i);
		cout << "AudioTrack Name: `" << T.name << "`\t@ " << T.startTime << " --> " << T.endTime << " (" << T.clips.size() << " clips, color=" << T.color << ")" << endl;

		for (unsigned int j = 0; j < T.clips.size(); j++)
		{
			const ofxAbletonLiveSet::AudioClip& m = T.clips.at(j);
			cout << "AudioClip Name: `" << m.name << "`\t@" << m.time << " --> " << m.time+m.duration << endl;
			cout << "AudioClip File: `" << m.file.relativePath << m.file.name << "`\tSize=" << m.file.size << endl;
		}

		cout << "===" << endl << endl;
	}
	
	ofExit();
}
