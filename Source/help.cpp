/**
 * @file help.cpp
 *
 * Implementation of the in-game help text.
 */
#include <string>
#include <vector>

#include "DiabloUI/ui_flags.hpp"
#include "control.h"
#include "engine/render/text_render.hpp"
#include "help.h"
#include "init.h"
#include "minitext.h"
#include "stores.h"
#include "utils/language.h"
#include "utils/stdcompat/string_view.hpp"

namespace devilution {

help_id HelpFlag;
help_id PrevHelpFlag;

namespace {

unsigned int SkipLines;

const char *const HelpText[] = {
	N_("$Keyboard Shortcuts:"),
	N_("F1:    Open Help Screen"),
	N_("F2:    Open Shirines Info"),
	N_("Esc:   Display Main Menu"),
	N_("Tab:   Display Auto-map"),
	N_("Space: Hide all info screens"),
	N_("S: Open Speedbook"),
	N_("B: Open Spellbook"),
	N_("I: Open Inventory screen"),
	N_("C: Open Character screen"),
	N_("Q: Open Quest log"),
	N_("F: Reduce screen brightness"),
	N_("G: Increase screen brightness"),
	N_("Z: Zoom Game Screen"),
	N_("+ / -: Zoom Automap"),
	N_("1 - 8: Use Belt item"),
	N_("F5, F6, F7, F8:     Set hotkey for skill or spell"),
	N_("Shift + Left Mouse Button: Attack without moving"),
	N_("Shift + Left Mouse Button (on character screen): Assign all stat points"),
	N_("Shift + Left Mouse Button (on inventory): Move item to belt or equip/unequip item"),
	N_("Shift + Left Mouse Button (on belt): Move item to inventory"),
	"",
	N_("$Movement:"),
	N_("If you hold the mouse button down while moving, the character "
	   "will continue to move in that direction."),
	"",
	N_("$Combat:"),
	N_("Holding down the shift key and then left-clicking allows the "
	   "character to attack without moving."),
	"",
	N_("$Auto-map:"),
	N_("To access the auto-map, click the 'MAP' button on the "
	   "Information Bar or press 'TAB' on the keyboard. Zooming in and "
	   "out of the map is done with the + and - keys. Scrolling the map "
	   "uses the arrow keys."),
	"",
	N_("$Picking up Objects:"),
	N_("Useable items that are small in size, such as potions or scrolls, "
	   "are automatically placed in your 'belt' located at the top of "
	   "the Interface bar . When an item is placed in the belt, a small "
	   "number appears in that box. Items may be used by either pressing "
	   "the corresponding number or right-clicking on the item."),
	"",
	N_("$Gold:"),
	N_("You can select a specific amount of gold to drop by "
	   "right-clicking on a pile of gold in your inventory."),
	"",
	N_("$Skills & Spells:"),
	N_("You can access your list of skills and spells by left-clicking on "
	   "the 'SPELLS' button in the interface bar. Memorized spells and "
	   "those available through staffs are listed here. Left-clicking on "
	   "the spell you wish to cast will ready the spell. A readied spell "
	   "may be cast by simply right-clicking in the play area."),
	"",
	N_("$Using the Speedbook for Spells:"),
	N_("Left-clicking on the 'readied spell' button will open the 'Speedbook' "
	   "which allows you to select a skill or spell for immediate use. "
	   "To use a readied skill or spell, simply right-click in the main play "
	   "area."),
	N_("Shift + Left-clicking on the 'select current spell' button will clear the readied spell."),
	"",
	N_("$Setting Spell Hotkeys:"),
	N_("You can assign up to four Hotkeys for skills, spells or scrolls. "
	   "Start by opening the 'speedbook' as described in the section above. "
	   "Press the F5, F6, F7 or F8 keys after highlighting the spell you "
	   "wish to assign."),
	"",
	N_("$Spell Books:"),
	N_("Reading more than one book increases your knowledge of that "
	   "spell, allowing you to cast the spell more effectively."),
};

const char *const DiabloShrinesText[] = {
	N_("$Abandoned Shrine"),
	N_("Text:  The hands of men may be guided by fate"),
	N_("Effect: +2 Dexterity"),
	"",
	N_("$Blood Fountain"),
	N_("Text:  None"),
	N_("Effect: Each click heals 1 HP"),
	"",
	N_("$Cauldron"),
	N_("Text:  Random"),
	N_("Effect: Random"),
	"",
	N_("$Creepy Shrine"),
	N_("Text:  Strength is bolstered by heavenly faith"),
	N_("Effect: +2 Strength"),
	"",
	N_("$Cryptic Shrine"),
	N_("Text:  Arcane power brings destruction"),
	N_("Effect: Restores Mana and casts Nova"),
	"",
	N_("$Divine Shrine"),
	N_("Text:  Drink and be Refreshed"),
	N_("Effect: Restores HP and Mana, drops 2 potions"),
	"",
	N_("$Eerie Shrine"),
	N_("Text:  Knowledge and wisdom at the cost of self"),
	N_("Effect: +2 Magic"),
	"",
	N_("$Eldritch Shrine"),
	N_("Text:  Crimson and azure become as the sun"),
	N_("Effect: Changes all potions into rejuvenation potions"),
	"",
	N_("$Enchanted Shrine"),
	N_("Text:  Magic is not always what it seems to be"),
	N_("Effect: -2 to a random spell, +1 to all other spells if possible"),
	"",
	N_("$Fascinating Shrine"),
	N_("Text:  Intensity comes at the cost of wisdom"),
	N_("Effect: -10% of Max Mana, +2 lvl to Firebolt spell"),
	"",
	N_("$Fountain of Tears"),
	N_("Text:  None"),
	N_("Effect: -1 to a random attribute, +1 to an other"),
	"",
	N_("$Glimmering Shrine"),
	N_("Text:  Mysteries are revealed in the light of reason"),
	N_("Effect: Identifies all items in inventory"),
	"",
	N_("$Goat Shrine"),
	N_("Text:  Random"),
	N_("Effect: Random"),
	"",
	N_("$Gloomy Shrine"),
	N_("Text:  Those who defend seldom attack"),
	N_("Effect: +2 AC to all armors, helms and shields in inventory"),
	N_("      -1 to max damage to all weapons in inventory"),
	N_("Note:  Single-player only"),
	"",
	N_("$Hidden Shrine"),
	N_("Text:  New strength is forged through destruction"),
	N_("Effect: -10 max durability to a random equipped item"),
	N_("      +10 max durability to all others"),
	"",
	N_("$Holy Shrine"),
	N_("Text:  Wherever you go, there you are"),
	N_("Effect: Casts Phasing"),
	"",
	N_("$Magical Shrine"),
	N_("Text:  While the spirit is vigilant the body thrives"),
	N_("Effect: Casts Mana Shield"),
	"",
	N_("$Murky Pool"),
	N_("Text:  None"),
	N_("Effect: Casts Infravision"),
	"",
	N_("$Mysterious Shrine"),
	N_("Text:  Some are weakened as one grows strong"),
	N_("Effect: +5 to a random attribute. -1 to all other"),
	"",
	N_("$Ornate Shrine"),
	N_("Text:  Salvation comes at the cost of Wisdom"),
	N_("Effect: -10% of Max Mana, +2 lvl to Holy Bolt spell"),
	"",
	N_("$Purifying Spring"),
	N_("Text:  None"),
	N_("Effect: Each click restores 1 Mana point"),
	"",
	N_("$Quiet Shrine"),
	N_("Text:  The essence of life flows from within"),
	N_("Effect: +2 Vitality"),
	"",
	N_("$Religious Shrine"),
	N_("Text:  Time cannot diminish the power of steel"),
	N_("Effect: Repairs all items"),
	"",
	N_("$Sacred Shrine"),
	N_("Text:  Energy comes at the cost of wisdom"),
	N_("Effect: -10% of Max Mana, +2 lvl to Charged Bolt spell"),
	"",
	N_("$Secluded Shrine"),
	N_("Text:  The way is made clear when viewed from above"),
	N_("Effect: Shows complete map of the current level"),
	"",
	N_("$Spiritual Shrine"),
	N_("Text:  Riches abound when least expected"),
	N_("Effect: Fill empty cases in inventory with some gold"),
	"",
	N_("$Spooky Shrine"),
	N_("Text:  Where avarice fails, patience brings reward (user)"),
	N_("      Blessed by a benevolent companion (others)"),
	N_("Effect: (user) No effect"),
	N_("      (others) Health and Mana are restored to full"),
	"",
	N_("$Stone Shrine"),
	N_("Text:  The powers of mana refocused renews"),
	N_("Effect: Recharges all staves"),
	"",
	N_("$Tainted Shrine"),
	N_("Text:  Those who are last may yet be first (user)"),
	N_("      Generosity brings its own reward (others)"),
	N_("Effect: (user) No effect"),
	N_("      (others) +1 to a random attribute, -1 to all other"),
	"",
	N_("$Thaumaturgic Shrine"),
	N_("Text:  What was once opened now is closed"),
	N_("Effect: Closes and refills all opened chests on current level"),
	N_("Note:  Single-player only"),
	"",
	N_("$Weird Shrine"),
	N_("Text:  The sword of justice is swift and sharp"),
	N_("Effect: +1 to max damage of all weapons in inventory"),
};

const char *const HellfireShrinesText[] = {
	N_("$Abandoned Shrine"),
	N_("Text:  The hands of men may be guided by fate"),
	N_("Effect: +2 Dexterity"),
	"",
	N_("$Blood Fountain"),
	N_("Text:  None"),
	N_("Effect: Each click heals 1 HP"),
	"",
	N_("$Cauldron"),
	N_("Text:  Random"),
	N_("Effect: Random"),
	"",
	N_("$Creepy Shrine"),
	N_("Text:  Strength is bolstered by heavenly faith"),
	N_("Effect: +2 Strength"),
	"",
	N_("$Cryptic Shrine"),
	N_("Text:  Arcane power brings destruction"),
	N_("Effect: Restores Mana and casts Nova"),
	"",
	N_("$Divine Shrine"),
	N_("Text:  Drink and be Refreshed"),
	N_("Effect: Restores HP and Mana, drops 2 potions"),
	"",
	N_("$Eerie Shrine"),
	N_("Text:  Knowledge and wisdom at the cost of self"),
	N_("Effect: +2 Magic"),
	"",
	N_("$Eldritch Shrine"),
	N_("Text:  Crimson and azure become as the sun"),
	N_("Effect: Changes all potions into rejuvenation potions"),
	"",
	N_("$Enchanted Shrine"),
	N_("Text:  Magic is not always what it seems to be"),
	N_("Effect: -2 to a random spell, +1 to all other spells if possible"),
	"",
	N_("$Fascinating Shrine"),
	N_("Text:  Intensity comes at the cost of wisdom"),
	N_("Effect: -10% of Max Mana, +2 lvl to Firebolt spell"),
	"",
	N_("$Fountain of Tears"),
	N_("Text:  None"),
	N_("Effect: -1 to a random attribute, +1 to an other"),
	"",
	N_("$Glimmering Shrine"),
	N_("Text:  Mysteries are revealed in the light of reason"),
	N_("Effect: Identifies all items in inventory"),
	"",
	N_("$Goat Shrine"),
	N_("Text:  Random"),
	N_("Effect: Random"),
	"",
	N_("$Gloomy Shrine"),
	N_("Text:  Those who defend seldom attack"),
	N_("Effect: +2 AC to all armors, helms and shields in inventory"),
	N_("      -1 to max damage to all weapons in inventory"),
	N_("Note:  Single-player only"),
	"",
	N_("$Glowing Shrine"),
	N_("Text:  Knowledge is power"),
	N_("Effect: +5 Magic, -5% experience"),
	"",
	N_("$Hidden Shrine"),
	N_("Text:  New strength is forged through destruction"),
	N_("Effect: -10 max durability to a random equipped item"),
	N_("      +10 max durability to all others"),
	"",
	N_("$Holy Shrine"),
	N_("Text:  Wherever you go, there you are"),
	N_("Effect: Casts Phasing"),
	"",
	N_("$Magical Shrine"),
	N_("Text:  While the spirit is vigilant the body thrives"),
	N_("Effect: Casts Mana Shield"),
	"",
	N_("$Mendicant's Shrine"),
	N_("Text:  Give and you shall receive"),
	N_("Effect: Converts half of your gold to experience"),
	"",
	N_("$Murky Pool"),
	N_("Text:  None"),
	N_("Effect: Casts Infravision"),
	"",
	N_("$Murphy's Shrine"),
	N_("Text:  That which can break, will"),
	N_("Effect: 1 to 3 chance to lose 50% durability,"),
	N_("      of a random equipped item,"),
	N_("      or takes away 1/3 of the gold in inventory"),
	"",
	N_("$Mysterious Shrine"),
	N_("Text:  Some are weakened as one grows strong"),
	N_("Effect: +5 to a random attribute. -1 to all other"),
	"",
	N_("$Ornate Shrine"),
	N_("Text:  Salvation comes at the cost of Wisdom"),
	N_("Effect: -10% of Max Mana, +2 lvl to Holy Bolt spell"),
	"",
	N_("$Oily Shrine"),
	N_("Text:  That which does not kill you..."),
	N_("Effect: +2 Strength (Warrior) / +2 Dexterity (Rogue)"),
	N_("      +2 Magic (Sorcerer) / +1 Strength and +1 Dexterity (Monk)"),
	N_("      +1 Dexterity and +1 Magic (Bard) / +2 Vitality (Barbarian)"),
	"",
	N_("$Purifying Spring"),
	N_("Text:  None"),
	N_("Effect: Each click restores 1 Mana point"),
	"",
	N_("$Quiet Shrine"),
	N_("Text:  The essence of life flows from within"),
	N_("Effect: +2 Vitality"),
	"",
	N_("$Religious Shrine"),
	N_("Text:  Time cannot diminish the power of steel"),
	N_("Effect: Repairs all items"),
	"",
	N_("$Sacred Shrine"),
	N_("Text:  Energy comes at the cost of wisdom"),
	N_("Effect: -10% of Max Mana, +2 lvl to Charged Bolt spell"),
	"",
	N_("$Secluded Shrine"),
	N_("Text:  The way is made clear when viewed from above"),
	N_("Effect: Shows complete map of the current level"),
	"",
	N_("$Shimmering Shrine"),
	N_("Text:  Spiritual energy is restored"),
	N_("Effect: Restores Mana to full"),
	"",
	N_("$Solar Shrine"),
	N_("12pm - 6pm"),
	N_("Text:  You feel stronger"),
	N_("Effect: +2 Strength"),
	N_("6pm - 8pm"),
	N_("Text:  You feel wiser"),
	N_("Effect: +2 Magic"),
	N_("8pm - 4am"),
	N_("Text:  You feel refreshed"),
	N_("Effect: +2 Vitality"),
	N_("4am - 12pm"),
	N_("Text:  You feel more agile"),
	N_("Effect: +2 Dexterity"),
	"",
	N_("$Sparkling Shrine"),
	N_("Text:  Some experience is gained by touch"),
	N_("Effect: Gives 1000x(dungeon level) experience points, casts a Flash"),
	"",
	N_("$Spiritual Shrine"),
	N_("Text:  Riches abound when least expected"),
	N_("Effect: Fill empty cases in inventory with some gold"),
	"",
	N_("$Spooky Shrine"),
	N_("Text:  Where avarice fails, patience brings reward (user)"),
	N_("      Blessed by a benevolent companion (others)"),
	N_("Effect: (user) No effect"),
	N_("      (others) Health and Mana are restored to full"),
	"",
	N_("$Stone Shrine"),
	N_("Text:  The powers of mana refocused renews"),
	N_("Effect: Recharges all staves"),
	"",
	N_("$Tainted Shrine"),
	N_("Text:  Those who are last may yet be first (user)"),
	N_("      Generosity brings its own reward (others)"),
	N_("Effect: (user) No effect"),
	N_("      (others) +1 to a random attribute, -1 to all other"),
	"",
	N_("$Thaumaturgic Shrine"),
	N_("Text:  What was once opened now is closed"),
	N_("Effect: Closes and refills all opened chests on current level"),
	N_("Note:  Single-player only"),
	"",
	N_("$Town Shrine"),
	N_("Text:  There's no place like home"),
	N_("Effect: Casts a Town Portal"),
	"",
	N_("$Weird Shrine"),
	N_("Text:  The sword of justice is swift and sharp"),
	N_("Effect: +1 to max damage of all weapons in inventory"),
};

std::vector<std::string> HelpTextLines;
std::vector<std::string> ShrinesTextLines;

constexpr int PaddingTop = 32;
constexpr int PaddingLeft = 32;

constexpr int PanelHeight = 297;
constexpr int ContentTextWidth = 577;

int LineHeight()
{
	return IsSmallFontTall() ? 18 : 14;
}

int BlankLineHeight()
{
	return 12;
}

int DividerLineMarginY()
{
	return BlankLineHeight() / 2;
}

int HeaderHeight()
{
	return PaddingTop + LineHeight() + 2 * BlankLineHeight() + DividerLineMarginY();
}

int ContentPaddingY()
{
	return BlankLineHeight();
}

int ContentsTextHeight()
{
	return PanelHeight - HeaderHeight() - DividerLineMarginY() - 2 * ContentPaddingY() - BlankLineHeight();
}

int NumVisibleLines()
{
	return (ContentsTextHeight() - 1) / LineHeight() + 1; // Ceil
}

} // namespace

void InitHelp()
{
	static bool Initialized = false;
	if (Initialized)
		return;

	HelpFlag = help_id::HELP_NONE;
	PrevHelpFlag = HelpFlag;
	char tempString[4096];

	for (const auto *text : HelpText) {
		strcpy(tempString, _(text));

		const std::string paragraph = WordWrapString(tempString, 577);

		size_t previous = 0;
		while (true) {
			size_t next = paragraph.find('\n', previous);
			HelpTextLines.emplace_back(paragraph.substr(previous, next - previous));
			if (next == std::string::npos)
				break;
			previous = next + 1;
		}
	}

	auto shrinesText = gbIsHellfire ? HellfireShrinesText : DiabloShrinesText;
	for (const char* text = *shrinesText; text; text = *++shrinesText) {
		strcpy(tempString, _(text));

		const std::string paragraph = WordWrapString(tempString, 577);

		size_t previous = 0;
		while (true) {
			size_t next = paragraph.find('\n', previous);
			ShrinesTextLines.emplace_back(paragraph.substr(previous, next - previous));
			if (next == std::string::npos)
				break;
			previous = next + 1;
		}
	}

	Initialized = true;
}

void DrawHelp(const Surface &out)
{
	DrawSTextHelp();
	DrawQTextBack(out);

	const int lineHeight = LineHeight();
	const int blankLineHeight = BlankLineHeight();

	const char *title;
	if (HelpFlag == help_id::HELP_SHRINES)
		title = _("Shrines Info");
	else if (gbIsHellfire)
		title = gbIsSpawn ? _("Shareware Hellfire Help") : _("Hellfire Help");
	else
		title = gbIsSpawn ? _("Shareware Diablo Help") : _("Diablo Help");

	const int sx = PANEL_X + PaddingLeft;
	const int sy = UI_OFFSET_Y;

	DrawString(out, title,
	    { { sx, sy + PaddingTop + blankLineHeight }, { ContentTextWidth, lineHeight } },
	    UiFlags::ColorWhitegold | UiFlags::AlignCenter);

	const int titleBottom = sy + HeaderHeight();
	DrawSLine(out, titleBottom);

	const std::vector<std::string>& TextLines(HelpFlag == help_id::HELP_SHRINES ? ShrinesTextLines : HelpTextLines);
	const int numLines = NumVisibleLines();
	const int contentY = titleBottom + DividerLineMarginY() + ContentPaddingY();
	for (int i = 0; i < numLines; i++) {
		const string_view line = TextLines[i + SkipLines];
		if (line.empty()) {
			continue;
		}

		int offset = 0;
		UiFlags style = UiFlags::ColorWhite;
		if (line[0] == '$') {
			offset = 1;
			style = UiFlags::ColorBlue;
		}

		DrawString(out, line.substr(offset), { { sx, contentY + i * lineHeight }, { ContentTextWidth, lineHeight } }, style, /*spacing=*/1, lineHeight);
	}

	DrawString(out, _("Press ESC to end or the arrow keys to scroll."),
	    { { sx, contentY + ContentsTextHeight() + ContentPaddingY() + blankLineHeight }, { ContentTextWidth, lineHeight } },
	    UiFlags::ColorWhitegold | UiFlags::AlignCenter);
}

void DisplayHelp(help_id page)
{
	HelpFlag = page;
	if (HelpFlag != help_id::HELP_NONE && PrevHelpFlag != HelpFlag) {
		SkipLines = 0;
		PrevHelpFlag = HelpFlag;
	}
}

void HelpScrollUp()
{
	if (SkipLines > 0)
		SkipLines--;
}

void HelpScrollDown()
{
	const std::vector<std::string>& TextLines(HelpFlag == help_id::HELP_SHRINES ? ShrinesTextLines : HelpTextLines);
	if (SkipLines + NumVisibleLines() < TextLines.size())
		SkipLines++;
}

void HelpPageUp()
{
	const unsigned int visLines = NumVisibleLines();
	if (SkipLines >= visLines)
		SkipLines -= visLines;
	else
		SkipLines = 0;
}

void HelpPageDown()
{
	const std::vector<std::string>& TextLines(HelpFlag == help_id::HELP_SHRINES ? ShrinesTextLines : HelpTextLines);
	const unsigned int visLines = NumVisibleLines();
	if (SkipLines + 2 * visLines < TextLines.size())
		SkipLines += visLines;
	else
		SkipLines = TextLines.size() - visLines;
}

} // namespace devilution
