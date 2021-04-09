/**
 * @file help.h
 *
 * Interface of the in-game help text.
 */
#pragma once

namespace devilution {

#ifdef __cplusplus
extern "C" {
#endif

typedef enum help_flag {
    HLP_NONE,
	HLP_MAIN,
    HLP_SHRINES,
} help_flag;

extern enum help_flag helpflag;

void InitHelp();
void DrawHelp(CelOutputBuffer out);
void DisplayHelp(enum help_flag helptype);
void HelpScrollUp();
void HelpScrollDown();
void HelpPageUp();
void HelpPageDown();

#ifdef __cplusplus
}
#endif

}
