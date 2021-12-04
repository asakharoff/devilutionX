/**
 * @file help.h
 *
 * Interface of the in-game help text.
 */
#pragma once

#include "engine.h"

namespace devilution {

enum help_id : uint8_t {
	HELP_NONE,
	HELP_MAIN,
    HELP_SHRINES,
};

extern help_id HelpFlag;

void InitHelp();
void DrawHelp(const Surface &out);
void DisplayHelp(help_id page);
void HelpScrollUp();
void HelpScrollDown();
void HelpPageUp();
void HelpPageDown();

} // namespace devilution
