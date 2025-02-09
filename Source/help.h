/**
 * @file help.h
 *
 * Interface of the in-game help text.
 */
#pragma once

#include "engine/surface.hpp"

namespace devilution {

enum HelpType : uint8_t {
	HelpTypeNone,
	HelpTypeMain,
    HelpTypeShrines,
};

extern HelpType HelpFlag;

void InitHelp();
void DrawHelp(const Surface &out);
void DisplayHelp(HelpType helpType);
void HelpScrollUp();
void HelpScrollDown();
void HelpPageUp();
void HelpPageDown();

} // namespace devilution
