#include "model/drum/jb_core_midi.h"

#include "util/cfunctions.h"
#include <algorithm>
#include <array>
#include <cstring>

namespace {

struct DrumGroup {
	uint8_t firstNote;
	uint8_t lastNote;
	char const* name;
	char const* const* voiceNames;
};

constexpr std::array<char const*, 14> kJB808VoiceNames = {
    "BASS DRUM",
    "SNARE DRUM",
    "CLOSED HAT",
    "OPEN HAT",
    "RIMSHOT",
    "HANDCLAP",
    "CYMBAL",
    "COWBELL",
    "LOW TOM",
    "MID TOM",
    "HIGH TOM",
    "LOW CONGA",
    "MID CONGA",
    "HIGH CONGA",
};

constexpr std::array<DrumGroup, 1> kJBCoreGroups = {{
    {
        .firstNote = 0,
        .lastNote = 13,
        .name = "JB-808",
        .voiceNames = kJB808VoiceNames.data(),
    },
}};

enum class ParameterDisplayType : uint8_t {
	NUMBER,
	SEMITONES,
	PAN,
};

struct ParameterInfo {
	char const* name;
	uint8_t editorMaxValue;
	ParameterDisplayType displayType;
};

// Every JB Core drum repeats this A-D topology across its four consecutive CCs
// These limits apply only to GUI editing; the stored and transmitted MIDI values remain 0-127
constexpr std::array<ParameterInfo, 4> kJBCoreParameters = {{
    {"Tune", 12, ParameterDisplayType::SEMITONES},
    {"Tone", 8, ParameterDisplayType::NUMBER},
    {"Decay", 8, ParameterDisplayType::NUMBER},
    {"Pan", 127, ParameterDisplayType::PAN},
}};

} // namespace

namespace jb_core {

DrumInfo getDrumInfo(uint8_t note) {
	for (DrumGroup const& group : kJBCoreGroups) {
		if (note >= group.firstNote && note <= group.lastNote) {
			return {
			    group.name,
			    group.voiceNames[note - group.firstNote],
			};
		}
	}

	return {};
}

char const* getParameterName(uint8_t parameter) {
	if (parameter >= kJBCoreParameters.size()) {
		return nullptr;
	}

	return kJBCoreParameters[parameter].name;
}

int32_t clampEditorValue(uint8_t parameter, int32_t value) {
	if (parameter >= kJBCoreParameters.size()) {
		return value;
	}

	return std::clamp<int32_t>(value, 0, kJBCoreParameters[parameter].editorMaxValue);
}

void formatDisplayValue(uint8_t parameter, uint8_t value, char* buffer) {
	if (parameter >= kJBCoreParameters.size()) {
		intToString(value, buffer, 1);
		return;
	}

	switch (kJBCoreParameters[parameter].displayType) {
	case ParameterDisplayType::SEMITONES:
		buffer[0] = '+';
		intToString(value, buffer + 1, 1);
		strcat(buffer, "st");
		break;

	case ParameterDisplayType::PAN:
		if (value == 64) {
			strcpy(buffer, "center");
		}
		else {
			// MIDI pan has 64 steps left of center and 63 steps right of center
			int32_t percentage = value < 64 ? ((64 - value) * 100 + 32) / 64 : ((value - 64) * 100 + 31) / 63;
			intToString(percentage, buffer, 1);
			strcat(buffer, value < 64 ? "L" : "R");
		}
		break;

	case ParameterDisplayType::NUMBER:
		intToString(value, buffer, 1);
		break;
	}
}

} // namespace jb_core
