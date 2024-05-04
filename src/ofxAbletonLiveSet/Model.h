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

struct AbletonColorSwatch {

	// Docs
	// There's no docs on the colorIndex value, but it seems to match the M4L swatch.
	// M4L docs say it's a bitmask, maybe different from OF's one : (0x00rrggbb or (2^16 * red) + (2^8) * green + blue) ?
	// Src: https://docs.cycling74.com/max5/refpages/m4l-ref/m4l_live_object_model.html#Clip
	// Here's an image of them : https://remotify.io/community/question/how-display-ableton-track-colors-midi-controller-pads
	//		but the numbers are different; see table below with reverse-engineered color values: (xxx = unverified)
	// There are 5 rows, 14 columns. Colors are quite ordered so there could be an algorithm behind it.
	// First ROW : START - 140 - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - 152 - 153 - END
	// 2nd   ROW : START - 154 - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - END
	// 3rd   ROW : START - 168 - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - END
	// 4th   ROW : START - 182 - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - xxx - END
	// Last  ROW : START - 196 - 197 - 198 - 199 - 278 - 279 - 280 - 281 - 282 - 283 - 284 - 285 - 286 - 287 - END

	static ofFloatColor getColor(unsigned int _color){
		unsigned int normalizedIndex = (_color-((_color<200)?140:(140+278-200))); // reverse engineered formulae
		return ofFloatColor::fromHex( Colors[normalizedIndex%70] );
	}
	static const int Colors[70];
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

struct FileInfo {
	unsigned int size;
	std::string name;
	std::string relativePath;
	// todo, available data: lastmodified, Absolute path, type
};

struct Volume {
	float manual;
	Automation automation;
	bool isManual() const {
		return automation.events.size()==0;
	}
};

struct Clip {
	Time time;
	Time endtime;
	Time duration;

	string name;
	int color;

	string annotation;

	virtual ~Clip() = default; // To support polymorphism
};

struct MidiClip : public Clip {
	MidiClipLoop loop;
	vector<Note> notes;
	vector<Automation> envelopes;
};

struct AudioClip : public Clip  {
	// todo : AudioClip/SampleRef holds file name, size, relPath and absPathHint info
	FileInfo file;
};

struct Track {
	string name;
	int color;
	Volume volume;
	bool on;

	// Todo: TrackGroupId
	//TimeSignature timeSignature;
	//Time startTime;
	//Time endTime;
};

struct MidiTrack : public Track {
	TimeSignature timeSignature;
	vector<MidiClip> clips;
};

struct AudioTrack : public Track {
	vector<AudioClip> clips;
};

struct Locator {
	Time time;
	string name;
	string annotation;
};

struct LiveSet {
	string name;
	string userName; // User-defined project name !
	string annotation;
	string loadedFile;
	bool loadedFileIsRelative;
	bool isLoaded() const {
		return (!loadedFile.empty());
	}
	Tempo tempo;
	vector<Locator> locators;
	vector<MidiTrack> miditracks;
	vector<AudioTrack> audiotracks;

	float loopStartOffset;
	float loopDuration;
	bool loopOn;

	struct AbletonVersion_ {
		unsigned int versionMajor;
		std::string versionMinor;
		std::string creator;
	} abletonVersion;
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

struct LSTrackEvent {
	LSTrackEvent(string _trackName, AudioClip _audioClip, int _nthTrack, int _nthClipInTrack) : audioClip(_audioClip), trackName(_trackName), nthTrack(_nthTrack), nthClipInTrack(_nthClipInTrack) {};
	
	string trackName;
	int nthTrack;
	int nthClipInTrack;
	AudioClip audioClip;
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
