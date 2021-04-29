/**
 * @file track.h
 *
 * Interface of functionality tracking what the mouse cursor is pointing at.
 */
#pragma once

namespace devilution {

void track_process();
void track_lmb_loc(_cmd_id bCmd, BYTE x, BYTE y);
void track_lmb_param1(_cmd_id bCmd, WORD wParam1);
void track_repeat_walk(bool rep);
bool track_isscrolling();

} // namespace devilution
