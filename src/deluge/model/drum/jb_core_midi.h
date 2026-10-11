#pragma once

#include <cstdint>

namespace jb_core {

struct DrumInfo {
	char const* groupName;
	char const* voiceName;

	explicit operator bool() const {
		return groupName && voiceName;
	}
};

// Return the drum group and voice name for a given MIDI note number
DrumInfo getDrumInfo(uint8_t note);

// Return the fixed target name for one parameter in each four-CC drum group
char const* getParameterName(uint8_t parameter);

// Limit values entered with the JB Core trigger-editing controls without changing
// the 0-127 range used by note storage and MIDI transmission
int32_t clampEditorValue(uint8_t parameter, int32_t value);

// Format a JB Core value for its GUI popup (semitones, number, or bipolar pan)
void formatDisplayValue(uint8_t parameter, uint8_t value, char* buffer);

// Disable Automation View for now, since it is not yet fully implemented
// This will be re-enabled in the future
// Change to true to enable Automation View
constexpr bool kAutomationViewEnabled = false;

} // namespace jb_core
