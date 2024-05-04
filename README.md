ofxAbletonLiveSet
=================

Ableton Live Set Project File parser.  
Loads an `.als` (or `.xml`) Ableton Live set into useable OpenFrameworks variables.  
Useful for generating visuals that synchronise with audio.

_Note: This plugin doesn't let you control Ableton via OSC. (use [ofxAbleton](https://github.com/tassock/ofxAbleton) or [ofxAbletonLive](http://github.com/genekogan/ofxAbletonLive) for that)_

------

## Dependencies
- (optional) ofxPoco for more precise event notification and zip decompression on all platforms.
- If using OF < 0.10 you'll need to install [ofxPugiXml](http://github.com/bakercp/ofxPugiXML).

###### Configuration

**Disabling ofxPoco**
In you project or addon that uses ofxAbletonLiveSet, you can define the `OFX_ALS_WITHOUT_POCO` compiler flag to disable usage of ofxPoco, which the plugin relies on historically.  
To not break existing projects, ofxPoco is still enabled by default; but it's recommended to disable it if you don't need very precise real-time events.

## Usage
Please refer to the __project examples__ for detailed information.  

###### Parse ALS file
````cpp
ofxAbletonLiveSet::LiveSet LS;
ofxAbletonLiveSet::Parser parser(LS);
parser.open("Project.als");
// LS now contains various live set data
````

###### Events:
````cpp
ofxAbletonLiveSet::EventHandler eventHandler;
ofAddListener(ofx::AbletonLiveSet::EventHandler::noteEvent, this, &ofApp::noteEventListener);
eventHandler.enableNoteEvents(LS);

void ofApp::noteEventListener(const ofx::AbletonLiveSet::LSNoteEvent & noteEvent){
	cout << "Note Event: " << noteEvent.note.key << " @ " << noteEvent.note.time << endl;
}
````


_________
## Features  

ofxAbletonLiveSet is divided into multiple blocks that you stack together to match your needs :  
  1. A standardised ALS project model that you can query manually.  
  2. A parser that extracts the data from the XML data.  
	3. A database object that lets you easily access the data over time.  
	4. An event handler that emits ofEvents.  
	5. _(Soon!)_ An ofxImGui helper to display the data.  

###### File Loading
- `.als` files are compressed ZIP files. By default, ofxPoco is used to decompress the file contents to obtain the underlying XML tree.
- ofxPoco can be disabled via a compilation flag and loading will use a system call to decompress the file (osx+linux only).
- An alternatively is to load the project pre-decompressed. Rename `MyProject.als` to `.gz` and decompress it, then rename it to `MyProject.xml` which you can load.

###### Data Extraction
- __Project__ (Name, Program Creator)
- __MasterTrack__ (Bpm, Time Signature, Duration, Loop)
- __MidiTrack__ (Name, MidiClips, Envelopes, MidiNotes, Color, Volume, etc.)
- __AudioTrack__ (Name, AudioClips, Color, Volume, ClipFiles, etc.)
- __Locations__ (Sort of Ableton Markers)


###### Event Nofication  

By default, Poco is used for precise timing.  
If poco is disabled, the events are fired using `ofThread` and `ofGetElapsedofGetElapsedTimef`.
- Notify of Metronom events.
- Notify of MidiNote events.
- Notify of AudioClip events.

---------

## Not yet implemented
- Some plugins carry floatEvents or boolEvents or enumEvents  
- Some tracks can have WarpMarkers
- Tempo Automation _(when the pbm changes)_ not yet supported

Do you have other ideas ?  
Feel free to contribute! :D

# Credits
Initiated by [@Satoru Higa](https://github.com/satoruhiga/).  
Continued by [@Daan de Lange](https://github.com/Daandelange).
