// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for R.

#include <cassert>
#include <cstring>

#include <string>
#include <string_view>

#include "ILexer.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "WordList.h"
#include "LexAccessor.h"
#include "Accessor.h"
#include "StyleContext.h"
#include "CharacterSet.h"
#include "LexerModule.h"

using namespace Lexilla;

namespace {

struct EscapeSequence {
	int outerState = SCE_R_DEFAULT;
	int digitsLeft = 0;
	int numBase = 0;

	// highlight any character as escape sequence, no highlight for hex in '\u{hex}' or '\U{hex}'.
	void resetEscapeState(int state, int chNext) noexcept {
		outerState = state;
		digitsLeft = 1;
		numBase = 16;
		if (chNext == 'x') {
			digitsLeft = 3;
		} else if (chNext == 'u') {
			digitsLeft = 5;
		} else if (chNext == 'U') {
			digitsLeft = 9;
		} else if (IsOctalDigit(chNext)) {
			digitsLeft = 3;
			numBase = 8;
		}
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsHexDigit(ch);
	}
};

constexpr bool IsFormatSpecifier(char ch) noexcept {
	// copied from LexAwk
	return AnyOf(ch, 'a', 'A',
					'c',
					'd',
					'e', 'E',
					'f', 'F',
					'g', 'G',
					'i',
					'o',
					's',
					'u',
					'x', 'X');
}

// https://www.rdocumentation.org/packages/base/versions/3.6.2/topics/sprintf
inline Sci_Position CheckFormatSpecifier(const StyleContext &sc, LexAccessor &styler, bool insideUrl) noexcept {
	if (sc.chNext == '%') {
		return 2;
	}
	if (insideUrl && IsHexDigit(sc.chNext)) {
		// percent encoded URL string
		return 0;
	}
	if (IsASpaceOrTab(sc.chNext) && IsADigit(sc.chPrev)) {
		// ignore word after percent: "5% x"
		return 0;
	}

	// similar to LexAwk
	Sci_PositionU pos = sc.currentPos + 1;
	char ch = styler.SafeGetCharAt(pos);
	// argument
	while (IsADigit(ch)) {
		ch = styler.SafeGetCharAt(++pos);
	}
	if (ch == '$') {
		ch = styler.SafeGetCharAt(++pos);
	}
	// flags
	while (AnyOf(ch, '-', '+', ' ', '#', '0')) {
		ch = styler.SafeGetCharAt(++pos);
	}
	for (int i = 0; i < 2; i++) {
		// width
		const bool argument = ch == '*';
		if (argument) {
			ch = styler.SafeGetCharAt(++pos);
		}
		while (IsADigit(ch)) {
			ch = styler.SafeGetCharAt(++pos);
		}
		if (argument && ch == '$') {
			ch = styler.SafeGetCharAt(++pos);
		}
		// precision
		if (i == 0 && ch == '.') {
			ch = styler.SafeGetCharAt(++pos);
		} else {
			break;
		}
	}
	// conversion format specifier
	if (IsFormatSpecifier(ch)) {
		return pos - sc.currentPos + 1;
	}
	return 0;
}

void ColouriseRDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineComment = 0;
	int chBeforeIdentifier = 0;
	int visibleChars = 0;
	bool insideUrl = false;
	EscapeSequence escSeq;

	StyleContext sc(startPos, lengthDoc, initStyle, styler);

	while (sc.More()) {
		switch (sc.state) {
		case SCE_R_OPERATOR:
			sc.SetState(SCE_R_DEFAULT);
			break;

		case SCE_R_INFIX:
			if (sc.ch == '%') {
				sc.ForwardSetState(SCE_R_DEFAULT);
			} else if (sc.atLineStart) {
				sc.SetState(SCE_R_DEFAULT);
			}
			break;

		case SCE_R_NUMBER:
			if (!IsDecimalNumberEx(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_R_DEFAULT);
			}
			break;

		case SCE_R_IDENTIFIER:
			if (!IsIdentifierCharEx(sc.ch)) {
				if (sc.ch != '.' && chBeforeIdentifier != '.') {
					char s[128];
					sc.GetCurrent(s, sizeof(s));
					if (keywordLists[0]->InList(s)) {
						sc.ChangeState(SCE_R_KEYWORD);
					} else if (keywordLists[1]->InList(s)) {
						sc.ChangeState(SCE_R_WORD2);
					} else if (keywordLists[2]->InList(s)) {
						sc.ChangeState(SCE_R_WORD3);
					}
				}
				if (sc.state == SCE_R_IDENTIFIER && sc.GetLineNextChar() == '(') {
					sc.ChangeState(SCE_R_FUNCTION);
				}
				sc.SetState(SCE_R_DEFAULT);
			}
			break;

		case SCE_R_COMMENT:
		case SCE_R_DIRECTIVE:
			if (sc.atLineStart) {
				sc.SetState(SCE_R_DEFAULT);
			}
			break;

		case SCE_R_STRING:
		case SCE_R_STRING2:
			if (sc.ch == '\\') {
				escSeq.resetEscapeState(sc.state, sc.chNext);
				sc.SetState(SCE_R_ESCAPECHAR);
				sc.Forward();
			} else if (sc.ch == '%') {
				const Sci_Position length = CheckFormatSpecifier(sc, styler, insideUrl);
				if (length != 0) {
					const int state = sc.state;
					sc.SetState(SCE_R_FORMAT_SPECIFIER);
					sc.Advance(length);
					sc.SetState(state);
					continue;
				}
			} else if ((sc.state == SCE_R_STRING && sc.ch == '\'')
				|| (sc.state == SCE_R_STRING2 && sc.ch == '\"')) {
				sc.ForwardSetState(SCE_R_DEFAULT);
			} else if (sc.atLineStart) {
				sc.SetState(SCE_R_DEFAULT);
			} else if (sc.Match(':', '/', '/') && IsLowerCase(sc.chPrev)) {
				insideUrl = true;
			} else if (insideUrl && IsInvalidUrlChar(sc.ch)) {
				insideUrl = false;
			}
			break;

		case SCE_R_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_R_BACKTICKS:
			if (sc.ch == '`') {
				sc.ForwardSetState(SCE_R_DEFAULT);
			} else if (sc.atLineStart) {
				sc.SetState(SCE_R_DEFAULT);
			}
			break;
		}

		if (sc.state == SCE_R_DEFAULT) {
			if (sc.ch == '#') {
				if (visibleChars == 0 && sc.Match("#line")) {
					sc.SetState(SCE_R_DIRECTIVE);
				} else {
					sc.SetState(SCE_R_COMMENT);
					if (visibleChars == 0) {
						lineStateLineComment = SimpleLineStateMaskLineComment;
					}
				}
			} else if (sc.ch == '\"') {
				insideUrl = false;
				sc.SetState(SCE_R_STRING2);
			} else if (sc.ch == '\'') {
				insideUrl = false;
				sc.SetState(SCE_R_STRING);
			} else if (sc.ch == '`') {
				sc.SetState(SCE_R_BACKTICKS);
			} else if (IsNumberStartEx(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_R_NUMBER);
			} else if (IsIdentifierStartEx(sc.ch)) {
				chBeforeIdentifier = sc.chPrev;
				sc.SetState(SCE_R_IDENTIFIER);
			} else if (sc.ch == '%') {
				sc.SetState(SCE_R_INFIX);
			} else if (isoperator(sc.ch) || sc.ch == '$' || sc.ch == '@') {
				sc.SetState(SCE_R_OPERATOR);
			}
		}

		if (visibleChars == 0 && !isspacechar(sc.ch)) {
			visibleChars = 1;
		}
		if (sc.atLineEnd) {
			styler.SetLineState(sc.currentLine, lineStateLineComment);
			lineStateLineComment = 0;
			visibleChars = 0;
			insideUrl = false;
		}
		sc.Forward();
	}

	sc.Complete();
}

constexpr int GetLineCommentState(int lineState) noexcept {
	return lineState & SimpleLineStateMaskLineComment;
}

static_assert(SCE_R_OPERATOR == SCE_SIMPLE_OPERATOR);
}

namespace Lexilla {

void FoldSimpleDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int /*initStyle*/, LexerWordList /*keywordLists*/, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	int levelCurrent = SC_FOLDLEVELBASE;
	int lineCommentPrev = 0;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		lineCommentPrev = GetLineCommentState(styler.GetLineState(lineCurrent - 1));
	}

	int levelNext = levelCurrent;
	int lineCommentCurrent = GetLineCommentState(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const int style = styler.StyleAt(i);

		if (style == SCE_SIMPLE_OPERATOR) {
			const char ch = styler[i];
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
		}

		if (i == lineEndPos) {
			const int lineCommentNext = GetLineCommentState(styler.GetLineState(lineCurrent + 1));
			if (lineCommentCurrent) {
				levelNext += lineCommentNext - lineCommentPrev;
			}

			const int levelUse = levelCurrent;
			int lev = levelUse | levelNext << 16;
			if (levelUse < levelNext) {
				lev |= SC_FOLDLEVELHEADERFLAG;
			}
			if (lev != styler.LevelAt(lineCurrent)) {
				styler.SetLevel(lineCurrent, lev);
			}

			lineCurrent++;
			lineStartNext = styler.LineStart(lineCurrent + 1);
			lineEndPos = sci::min(lineStartNext, endPos) - 1;
			levelCurrent = levelNext;
			lineCommentPrev = lineCommentCurrent;
			lineCommentCurrent = lineCommentNext;
		}
	}
}

}

LexerModule lmR(SCLEX_R, ColouriseRDoc, "r", FoldSimpleDoc);
