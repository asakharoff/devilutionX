/**
 * @file help.h
 *
 * Interface of the in-game help text.
 */
#pragma once

#include "engine.h"

namespace devilution {


enum help_flag : uint8_t {
    HLP_NONE,
	HLP_MAIN,
    HLP_SHRINES,
};

extern enum help_flag helpflag;

void InitHelp();
void DrawHelp(const CelOutputBuffer &out);
void DisplayHelp(enum help_flag helptype);
void HelpScrollUp();
void HelpScrollDown();
void HelpPageUp();
void HelpPageDown();

} // namespace devilution
