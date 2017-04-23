#pragma once

#include <Poco/InflatingStream.h>

#include "Constants.h"
#include "Tempo.h"
#include "Model.h"

OFX_ALS_BEGIN_NAMESPACE

class Parser {
public:
	
	Parser(LiveSet &LS) : LS(LS) {}
	
	bool open(const string& path);
	
	bool isLoaded() const;
	string loadedLiveSet();
	
private:
	
	LiveSet &LS;
	int controller_target_offset;
	
	void parseGeneralInfo(const pugi::xml_document& doc);
	
	void parseTempo(const pugi::xml_document& doc);
	
	void parse(Tempo& tempo, const pugi::xml_node& node);
	
	//
	
	void parseMidiTrack(const pugi::xml_document& doc);

	void parse(MidiTrack& MT, const pugi::xml_node &node, RealTime offset);
	
	void parse(Note& note, const pugi::xml_node &node, RealTime offset);
	
	void parse(MidiClipLoop& loop, const pugi::xml_node &node, RealTime offset);

	void parse(MidiClip& MC, const pugi::xml_node &node, RealTime offset);

	//
	// todo: parse AudioTrack ? (not much insteresting information...)
	//
	void parseAudioTrack(const pugi::xml_document& doc);
	
	void parse(AudioTrack& AT, const pugi::xml_node &node, RealTime offset);
	
	void parse(AudioClip& AC, const pugi::xml_node &node, RealTime offset);
	//

	void parseLocator(const pugi::xml_document& doc);
	
	void parse(Locator& L, const pugi::xml_node& node, RealTime offset);

	//
	
	template <typename T>
	static bool sort_by_time(const T& v0, const T& v1) { return v0.time < v1.time; }
};

OFX_ALS_END_NAMESPACE
