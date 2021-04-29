/**
 * @file track.cpp
 *
 * Implementation of functionality tracking what the mouse cursor is pointing at.
 */
#include <SDL.h>

#include "cursor.h"
#include "player.h"

namespace devilution {

namespace {

bool sgbIsScrolling;
Uint32 sgdwLastWalk;
bool sgbIsWalking;
_cmd_id sgbCommand;
WORD sgwParam1;

} // namespace

void track_process()
{
	if (!sgbIsWalking)
		return;

	if (cursmx < 0 || cursmx >= MAXDUNX - 1 || cursmy < 0 || cursmy >= MAXDUNY - 1)
		return;

	if (sgbCommand == CMD_WALKXY && plr[myplr]. actionFrame <= 6 && plr[myplr]._pmode != PM_STAND)
		return;

	const Point target = plr[myplr].GetTargetPosition();
	if (cursmx != target.x || cursmy != target.y) {
		Uint32 tick = SDL_GetTicks();
		if ((int)(tick - sgdwLastWalk) >= gnTickDelay * 6) {
			sgdwLastWalk = tick;
			switch (sgbCommand) {
				case CMD_WALKXY:
					NetSendCmdLoc(myplr, true, CMD_WALKXY, cursmx, cursmy);
					if (!sgbIsScrolling)
						sgbIsScrolling = 1;
					break;
				case CMD_SATTACKXY:
				case CMD_RATTACKXY:
					NetSendCmdLoc(myplr, true, sgbCommand, cursmx, cursmy);
					break;
				case CMD_ATTACKID:
				case CMD_RATTACKID:
					if (pcursmonst == -1)
						return;
					NetSendCmdParam1(true, sgbCommand, pcursmonst);
					break;
			}
		}
	}
}

void track_lmb_loc(_cmd_id bCmd, BYTE x, BYTE y)
{
	NetSendCmdLoc(myplr, true, bCmd, x, y);
	sgbCommand = bCmd;
}

void track_lmb_param1(_cmd_id bCmd, WORD wParam1)
{
	NetSendCmdParam1(true, bCmd, wParam1);
	sgbCommand = bCmd;
	sgwParam1 = wParam1;
}

void track_repeat_walk(bool rep)
{
	if (sgbIsWalking == rep)
		return;

	sgbIsWalking = rep;
	if (rep) {
		sgbIsScrolling = false;
		sgdwLastWalk = SDL_GetTicks() - gnTickDelay;
	} else if (sgbIsScrolling) {
		sgbIsScrolling = false;
	}
}

bool track_isscrolling()
{
	return sgbIsScrolling;
}

} // namespace devilution
