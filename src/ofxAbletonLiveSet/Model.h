#pragma once

#include "Constants.h"
#include "time.h"
#include "Tempo.h"

OFX_ALS_BEGIN_NAMESPACE

typedef float Time;

struct MidiClipLoop {
	Time start;
	Time end;
	Time duration;
	Time relativeStart;
	Time outMarker;
	bool enabled;
};

struct Note {
	Time time;
	Time duration;
	float velocity;
	int key;
	
	Note(Time _time, Time _duration, float _velocity, int _key) : time(_time), duration(_duration), velocity(_velocity), key(_key) {};
	
	Note() : time(-1), duration(0), velocity(0), key(0) {};
	
	Note operator + (const Time& other) const{
		return Note( time + other, duration, time, key);
	}
};

struct Automation {
	int id;
	float getValueAt(float time) const;
	map<float, float> events;
};

struct TimeSignature {
	int numerator;
	int denominator;
	int pbm;
	
};

struct MidiClip {
	Time time;
	Time endtime;
	Time duration;
	
	string name;
	string annotation;
	int color;
	
	MidiClipLoop loop;
	vector<Note> notes;
	vector<Automation> envelopes;
};

struct Track {
	string name;
	int color;
};

struct MidiTrack : public Track {
	TimeSignature timeSignature;
	vector<MidiClip> clips;
};

struct Locator {
	Time time;
	string name;
	string annotation;
};

struct LiveSet {
	string name;
	string userName;
	string annotation;
	string loadedFile;
	bool isLoaded() const {
		return (!loadedFile.empty());
	}
	Tempo tempo;
	vector<Locator> locators;
	vector<MidiTrack> miditracks;
};

struct LSNoteEvent {
	LSNoteEvent(string _clipName, int _color, int _nthNote, int _nthNoteInClip, int _trackNb, Note _note) : clipName(_clipName), color(_color), nthNote(_nthNote), nthNoteInClip(_nthNoteInClip), trackNb(_trackNb), note(_note) {};
	
	string clipName;
	int color;
	int nthNote;
	int nthNoteInClip;
	int trackNb;
	Note note;
};

struct LSMetronomEvent {
	int barTime;
	float realTime;
	int bpm;
	bool isAccent;
	int trackNb;
	TimeSignature timeSignature;
};

OFX_ALS_END_NAMESPACE
