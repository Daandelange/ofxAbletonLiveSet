#pragma once

#include "ofConstants.h"

// In debug compilation, clarify where Pugi comes from.
#if defined(DEBUG) && !defined(OFX_ALS_NO_PUGI_DEBUG)
#   if OF_VERSION_MAJOR <= 0 && OF_VERSION_MINOR >= 10
#      pragma message "ofxAbletonLiveSet is using libPugi shipped within ofxXML since OF 0.10"
#   else
#      pragma message "ofxAbletonLiveSet is using libPugi shipped within ofxPugiXml, install ofxPugiXml if necessary."
#   endif
#endif

#include "pugixml.hpp"
