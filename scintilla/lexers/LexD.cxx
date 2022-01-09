// This file is part of Notepad2.
// See License.txt for details about distribution and modification.
//! Lexer for D.

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
#include "StringUtils.h"
#include "LexerModule.h"

using namespace Lexilla;

namespace {

struct EscapeSequence {
	int outerState = SCE_D_DEFAULT;
	int digitsLeft = 0;
	int numBase = 0;

	// highlight any character as escape sequence, no highlight for '\&NamedCharacterEntity;'.
	void resetEscapeState(int state, int chNext) noexcept {
		outerState = state;
		digitsLeft = 1;
		numBase = 16;
		if (chNext == 'U') {
			digitsLeft = 9;
		} else if (chNext == 'u') {
			digitsLeft = 5;
		} else if (chNext == 'x') {
			digitsLeft = 3;
		} else if (IsOctalDigit(chNext)) {
			digitsLeft = 3;
			numBase = 8;
		}
	}
	bool atEscapeEnd(int ch) noexcept {
		--digitsLeft;
		return digitsLeft <= 0 || !IsADigit(ch, numBase);
	}
};

enum {
	DLineStateMaskLineComment = 1,	// line comment
	DLineStateMaskImport = 1 << 1,	// import
	DLineStateMaskDelimitedString = 1 << 2,
};

enum class InlineAssembler {
	None = 0,
	Keyword,
	X86,
	Gcc,
};

enum class KeywordType {
	None = SCE_D_DEFAULT,
	Class = SCE_D_CLASS,
	Struct = SCE_D_STRUCT,
	Union = SCE_D_UNION,
	Interface = SCE_D_INTERFACE,
	Trait = SCE_D_TRAIT,
	Enum = SCE_D_ENUM,
	Label = SCE_D_LABEL,
	Return,
};

constexpr bool IsSpaceEquiv(int state) noexcept {
	return state <= SCE_D_TASKMARKER;
}

constexpr bool IsDStringPostfix(int ch) noexcept {
	return ch == 'c' || ch == 'w' || ch == 'd';
}

constexpr int GetMatchingDelimiter(int delimiter) noexcept {
	switch (delimiter) {
	case '[': return ']';
	case '(': return ')';
	case '{': return '}';
	case '<': return '>';
	default: return delimiter;
	}
}

constexpr bool IsDocumentSectionPrev(int ch) noexcept {
	return ch == ' ' || ch == '*' || ch == '+' || ch == '/';
}

// https://dlang.org/spec/ddoc.html
void HighlightDocumentSection(StyleContext &sc, int visibleChars) {
	// TODO: highlight macro
	if ((visibleChars == 0 || (visibleChars <= 3 && IsDocumentSectionPrev(sc.chPrev))) && IsAlpha(sc.ch)) {
		Sci_PositionU pos = sc.currentPos + 1;
		while (pos < sc.lineStartNext) {
			const char ch = sc.styler[pos++];
			if (!IsIdentifierStart(ch)) {
				if (ch == ':' || ch == '=') {
					const int state = sc.state;
					sc.SetState(SCE_D_COMMENTDOCWORD);
					sc.Advance(pos - sc.currentPos);
					sc.SetState(state);
				}
				break;
			}
		}
	}
}

constexpr bool IsFormatSpecifier(char ch) noexcept {
	return AnyOf(ch, 'a', 'A',
					'b',
					'c',
					'd',
					'e', 'E',
					'f', 'F',
					'g', 'G',
					'i',
					'o',
					'r',
					's',
					'u',
					'x', 'X');
}

// https://dlang.org/phobos/std_format.html
inline Sci_Position CheckFormatSpecifier(const StyleContext &sc, LexAccessor &styler, bool insideUrl) noexcept {
	if (sc.chNext == '%' || sc.chNext == '(' || sc.chNext == '|' || sc.chNext == ')') {
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

	Sci_PositionU pos = sc.currentPos + 1;
	// compound indicator
	if (sc.chNext == '-' && styler.SafeGetCharAt(pos + 1) == '(') {
		return 3;
	}
	// Parameters
	char ch = styler.SafeGetCharAt(pos);
	// Position
	if (IsADigit(ch)) {
		while (IsADigit(ch)) {
			ch = styler.SafeGetCharAt(++pos);
		}
		if (ch == ':') {
			ch = styler.SafeGetCharAt(++pos);
			while (IsADigit(ch)) {
				ch = styler.SafeGetCharAt(++pos);
			}
		}
		if (ch == '$') {
			ch = styler.SafeGetCharAt(++pos);
		}
	}
	// Flags
	while (AnyOf(ch, '-', '+', ' ', '0', '#', '=')) {
		ch = styler.SafeGetCharAt(++pos);
	}
	// Width
	for (int i = 0; i < 2; i++) {
		// OptionalPositionalInteger
		if (ch == '*') {
			ch = styler.SafeGetCharAt(++pos);
		} else {
			while (IsADigit(ch)) {
				ch = styler.SafeGetCharAt(++pos);
			}
		}
		if (ch == '*') {
			ch = styler.SafeGetCharAt(++pos);
		}
		while (IsADigit(ch)) {
			ch = styler.SafeGetCharAt(++pos);
		}
		if (ch == '$') {
			ch = styler.SafeGetCharAt(++pos);
		}
		// Precision
		if (i == 0) {
			if (ch == '.') {
				ch = styler.SafeGetCharAt(++pos);
			} else {
				break;
			}
		}
	}
	// Separator
	if (ch == ',') {
		// OptionalInteger
		ch = styler.SafeGetCharAt(++pos);
		if (ch == '*') {
			ch = styler.SafeGetCharAt(++pos);
		} else {
			while (IsADigit(ch)) {
				ch = styler.SafeGetCharAt(++pos);
			}
		}
		if (ch == '?') {
			ch = styler.SafeGetCharAt(++pos);
		}
	}
	// FormatIndicator
	if (IsFormatSpecifier(ch)) {
		return pos - sc.currentPos + 1;
	}

	// TODO: highlight format specifier for C printf()
	return 0;
}

bool HandleInnerStringStyle(StyleContext &sc, EscapeSequence &escSeq, bool &insideUrl) {
	if (sc.ch == '\\') {
		if (!(sc.state == SCE_D_RAWSTRING || sc.state == SCE_D_STRING_BT)) {
			escSeq.resetEscapeState(sc.state, sc.chNext);
			sc.SetState(SCE_D_ESCAPECHAR);
			sc.Forward();
		}
	} else if (sc.ch == '%') {
		const Sci_Position length = CheckFormatSpecifier(sc, sc.styler, insideUrl);
		if (length != 0) {
			const int state = sc.state;
			sc.SetState(SCE_D_FORMAT_SPECIFIER);
			sc.Advance(length);
			sc.SetState(state);
			return true;
		}
	} else if (sc.Match(':', '/', '/') && IsLowerCase(sc.chPrev)) {
		insideUrl = true;
	} else if (insideUrl && IsInvalidUrlChar(sc.ch)) {
		insideUrl = false;
	}
	return false;
}

void ColouriseDDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList keywordLists, Accessor &styler) {
	int lineStateLineType = 0;
	int nestedLevel = 0; // nested block comment level or nesting delimiter
	bool insideUrl = false;
	// TODO: highlight token string q{}

	KeywordType kwType = KeywordType::None;
	InlineAssembler asmState = InlineAssembler::None;

	int visibleChars = 0;
	int chBefore = 0;
	int visibleCharsBefore = 0;
	int chPrevNonWhite = 0;
	EscapeSequence escSeq;

	// for delimited strings
	std::string delimiters;
	int delimiterChar = 0;
	int matchingDelimiter = 0;

	if (startPos != 0) {
		// backtrack to restore delimiters
		BacktrackToStart(styler, DLineStateMaskDelimitedString, startPos, lengthDoc, initStyle);
	}

	StyleContext sc(startPos, lengthDoc, initStyle, styler);
	if (sc.currentLine > 0) {
		const int lineState = styler.GetLineState(sc.currentLine - 1);
		/*
		2: lineStateLineType
		1: DLineStateMaskDelimitedString
		2: asmState
		3: unused
		8: nestedLevel
		8: tokenLevel
		*/
		asmState = static_cast<InlineAssembler>((lineState >> 3) & 3);
		nestedLevel = (lineState >> 8) & 0xff;
	}
	if (startPos == 0 && sc.Match('#', '!')) {
		// Shell Shebang at beginning of file
		sc.SetState(SCE_D_COMMENTLINE);
		sc.Forward();
		lineStateLineType = DLineStateMaskLineComment;
	}

	while (sc.More()) {
		switch (sc.state) {
		case SCE_D_OPERATOR:
			sc.SetState(SCE_D_DEFAULT);
			break;

		case SCE_D_NUMBER:
			if (!IsDecimalNumberEx(sc.chPrev, sc.ch, sc.chNext)) {
				sc.SetState(SCE_D_DEFAULT);
			}
			break;

		case SCE_D_IDENTIFIER:
		case SCE_D_PREPROCESSOR:
		case SCE_D_ATTRIBUTE:
			if (!IsIdentifierCharEx(sc.ch)) {
				if (sc.state == SCE_D_IDENTIFIER) {
					char s[128];
					sc.GetCurrent(s, sizeof(s));
					if (keywordLists[0]->InList(s)) {
						sc.ChangeState(SCE_D_WORD);
						if (asmState == InlineAssembler::None) {
							if (StrEqual(s, "import")) {
								lineStateLineType = DLineStateMaskImport;
							} else if (StrEqualsAny(s, "class", "new")) {
								kwType = KeywordType::Class;
							} else if (StrEqual(s, "struct")) {
								kwType = KeywordType::Struct;
							} else if (StrEqual(s, "union")) {
								kwType = KeywordType::Union;
							} else if (StrEqual(s, "interface")) {
								kwType = KeywordType::Interface;
							} else if (StrEqual(s, "__traits")) {
								kwType = KeywordType::Trait;
							} else if (StrEqual(s, "enum")) {
								kwType = KeywordType::Enum;
							} else if (StrEqualsAny(s, "break", "goto")) {
								kwType = KeywordType::Label;
							} else if (StrEqual(s, "return")) {
								kwType = KeywordType::Return;
							} else if (StrEqual(s, "asm")) {
								asmState = InlineAssembler::Keyword;
							}
							if (kwType > KeywordType::None && kwType < KeywordType::Return) {
								const int chNext = sc.GetLineNextChar(kwType == KeywordType::Trait && sc.ch == '(');
								if (!IsIdentifierStartEx(chNext)) {
									kwType = KeywordType::None;
								}
							}
						}
					} else if (keywordLists[1]->InList(s)) {
						sc.ChangeState(SCE_D_WORD2);
					} else if (keywordLists[4]->InList(s)) {
						sc.ChangeState(SCE_D_CLASS);
					} else if (keywordLists[5]->InList(s)) {
						sc.ChangeState(SCE_D_STRUCT);
					} else if (keywordLists[6]->InList(s)) {
						sc.ChangeState(SCE_D_UNION);
					} else if (keywordLists[7]->InList(s)) {
						sc.ChangeState(SCE_D_INTERFACE);
					} else if (sc.ch == '!' && keywordLists[8]->InList(s)) {
						sc.ChangeState(SCE_D_TRAIT);
					} else if (keywordLists[9]->InList(s)) {
						sc.ChangeState(SCE_D_ENUM);
					} else if (keywordLists[10]->InList(s)) {
						sc.ChangeState(SCE_D_CONSTANT);
					} else if (asmState == InlineAssembler::X86) {
						if (keywordLists[11]->InList(s)) {
							sc.ChangeState(SCE_D_WORD);
						} else if (keywordLists[12]->InList(s)) {
							sc.ChangeState(SCE_D_ASM_REGISTER);
						} else if (visibleChars == sc.LengthCurrent() || AnyOf(chBefore, ';', ':', '{')) {
							sc.ChangeState((sc.ch == ':')? SCE_D_LABEL : SCE_D_ASM_INSTRUCTION);
						}
					} else if (sc.ch == ':') {
						if (visibleChars == sc.LengthCurrent()) {
							sc.ChangeState(SCE_D_LABEL);
						}
					} else if (sc.ch != '.') {
						if (kwType > KeywordType::None && kwType < KeywordType::Return) {
							sc.ChangeState(static_cast<int>(kwType));
						} else {
							const int chNext = sc.GetDocNextChar(sc.ch == '!');
							if (chNext == '(') {
								// type method()
								// type[] method()
								if (kwType != KeywordType::Return && (IsIdentifierCharEx(chBefore) || chBefore == ']')) {
									sc.ChangeState(SCE_D_FUNCTION_DEFINITION);
								} else {
									sc.ChangeState(SCE_D_FUNCTION);
								}
							} else if (sc.Match('[', ']') || IsIdentifierStartEx(chNext)) {
								// type[] identifier
								// type identifier
								sc.ChangeState(IsInterfaceName(s[0], s[1]) ? SCE_D_INTERFACE : SCE_D_CLASS);
							}
						}
					}
					if (sc.state != SCE_D_WORD && sc.ch != '.') {
						kwType = KeywordType::None;
					}
				}

				sc.SetState(SCE_D_DEFAULT);
			}
			break;

		case SCE_D_COMMENTLINE:
		case SCE_D_COMMENTLINEDOC:
			if (sc.atLineStart) {
				sc.SetState(SCE_D_DEFAULT);
			} else if (!HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_D_TASKMARKER)) {
				if (sc.state == SCE_D_COMMENTLINEDOC) {
					HighlightDocumentSection(sc, visibleChars);
				}
			}
			break;

		case SCE_D_COMMENTBLOCK:
		case SCE_D_COMMENTBLOCKDOC:
			if (sc.Match('*', '/')) {
				sc.Forward();
				sc.ForwardSetState(SCE_D_DEFAULT);
			} else if (HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_D_TASKMARKER)) {
				continue;
			} else if (sc.state == SCE_D_COMMENTBLOCKDOC) {
				HighlightDocumentSection(sc, visibleChars);
			}
			break;

		case SCE_D_NESTED_COMMENT:
		case SCE_D_NESTED_COMMENTDOC:
			if (sc.Match('+', '/')) {
				sc.Forward();
				--nestedLevel;
				if (nestedLevel == 0) {
					sc.ForwardSetState(SCE_D_DEFAULT);
				}
			} else if (sc.Match('/', '+')) {
				sc.Forward();
				++nestedLevel;
			} else if (HighlightTaskMarker(sc, visibleChars, visibleCharsBefore, SCE_D_TASKMARKER)) {
				continue;
			} else if (sc.state == SCE_D_NESTED_COMMENTDOC) {
				HighlightDocumentSection(sc, visibleChars);
			}
			break;

		case SCE_D_CHARACTER:
			if (sc.ch == '\'') {
				sc.ForwardSetState(SCE_D_DEFAULT);
			} else if (sc.ch == '\\' && !IsEOLChar(sc.chNext)) {
				escSeq.resetEscapeState(sc.state, sc.chNext);
				sc.SetState(SCE_D_ESCAPECHAR);
				sc.Forward();
			} else if (sc.atLineStart) {
				sc.SetState(SCE_D_DEFAULT);
			}
			break;

		case SCE_D_STRING:
		case SCE_D_HEXSTRING:
		case SCE_D_RAWSTRING:
		case SCE_D_STRING_BT:
			if ((sc.ch == '\"' && sc.state != SCE_D_STRING_BT) || (sc.ch == '`' && sc.state == SCE_D_STRING_BT)) {
				sc.Forward();
				if (IsDStringPostfix(sc.ch)) {
					sc.Forward();
				}
				sc.SetState(SCE_D_DEFAULT);
			} else if (HandleInnerStringStyle(sc, escSeq, insideUrl)) {
				continue;
			}
			break;

		case SCE_D_ESCAPECHAR:
			if (escSeq.atEscapeEnd(sc.ch)) {
				sc.SetState(escSeq.outerState);
				continue;
			}
			break;

		case SCE_D_DELIMITED_ID:
			if (!IsIdentifierCharEx(sc.ch)) {
				delimiters = styler.GetRange(styler.GetStartSegment(), sc.currentPos);
				sc.ChangeState(SCE_D_DELIMITED_STRING);
			}
			break;

		case SCE_D_DELIMITED_STRING:
			if (delimiters.empty()) {
				if (sc.ch == matchingDelimiter) {
					if (delimiterChar != matchingDelimiter) {
						--nestedLevel;
					}
					if (sc.chNext == '\"' && (nestedLevel == 0 || delimiterChar == matchingDelimiter)) {
						sc.Forward(2);
						if (IsDStringPostfix(sc.ch)) {
							sc.Forward();
						}
						sc.SetState(SCE_D_DEFAULT);
						break;
					}
				} else if (delimiterChar != matchingDelimiter && sc.ch == delimiterChar) {
					++nestedLevel;
				}
			} else if (sc.atLineStart) {
				if (sc.GetRelative(delimiters.length()) == '\"' && sc.Match(delimiters.c_str())) {
					sc.Advance(delimiters.length());
					sc.Forward();
					if (IsDStringPostfix(sc.ch)) {
						sc.Forward();
					}
					sc.SetState(SCE_D_DEFAULT);
					break;
				}
			}
			if (HandleInnerStringStyle(sc, escSeq, insideUrl)) {
				continue;
			}
			break;
		}

		if (sc.state == SCE_D_DEFAULT) {
			if (sc.ch == '/' && (sc.chNext == '/' || sc.chNext == '*')) {
				visibleCharsBefore = visibleChars;
				const int chNext = sc.chNext;
				sc.SetState((chNext == '/') ? SCE_D_COMMENTLINE : SCE_D_COMMENTBLOCK);
				sc.Forward(2);
				if (sc.ch == chNext) {
					sc.ChangeState((chNext == '/') ? SCE_D_COMMENTLINEDOC : SCE_D_COMMENTBLOCKDOC);
				}
				if (chNext == '/' && visibleChars == 0) {
					lineStateLineType = DLineStateMaskLineComment;
				}
				continue;
			}
			if (sc.Match('/', '+')) {
				nestedLevel = 1;
				visibleCharsBefore = visibleChars;
				sc.SetState(SCE_D_NESTED_COMMENT);
				sc.Forward();
				if (sc.chNext == '+') {
					sc.ChangeState(SCE_D_NESTED_COMMENTDOC);
				}
			} else if ((sc.ch == 'r' || sc.ch == 'x') && sc.chNext == '\"') {
				insideUrl = false;
				sc.SetState((sc.ch == 'r') ? SCE_D_RAWSTRING : SCE_D_HEXSTRING);
				sc.Forward();
			} else if (sc.ch == '`') {
				insideUrl = false;
				sc.SetState(SCE_D_STRING_BT);
			} else if (sc.ch == '"') {
				insideUrl = false;
				sc.SetState(SCE_D_STRING);
			} else if (sc.ch == '\'') {
				sc.SetState(SCE_D_CHARACTER);
			} else if (sc.Match('q', '\"')) {
				sc.SetState(SCE_D_DELIMITED_STRING);
				sc.Forward(2);
				insideUrl = false;
				nestedLevel = 0;
				delimiters.clear();
				delimiterChar = sc.ch;
				matchingDelimiter = GetMatchingDelimiter(delimiterChar);
				if (delimiterChar != matchingDelimiter) {
					nestedLevel = 1;
				} else if (IsIdentifierStartEx(delimiterChar)) {
					sc.SetState(SCE_D_DELIMITED_ID);
				}
			} else if (IsNumberStart(sc.ch, sc.chNext)) {
				sc.SetState(SCE_D_NUMBER);
			} else if ((sc.ch == '@' || sc.ch == '#') && IsIdentifierStartEx(sc.chNext)) {
				sc.SetState((sc.ch == '@') ? SCE_D_ATTRIBUTE : SCE_D_PREPROCESSOR);
			} else if (IsIdentifierStartEx(sc.ch)) {
				chBefore = chPrevNonWhite;
				sc.SetState(SCE_D_IDENTIFIER);
			} else if (isoperator(sc.ch) || sc.ch == '@' || sc.ch == '$') {
				sc.SetState(SCE_D_OPERATOR);
				if (asmState == InlineAssembler::Keyword && sc.ch == '{') {
					const int chNext = sc.GetDocNextChar(true);
					asmState = (chNext == '\"')? InlineAssembler::Gcc : InlineAssembler::X86;
				} else if (asmState > InlineAssembler::Keyword && sc.ch == '}') {
					asmState = InlineAssembler::None;
				}
			}
		}

		if (!isspacechar(sc.ch)) {
			visibleChars++;
			if (!IsSpaceEquiv(sc.state)) {
				chPrevNonWhite = sc.ch;
			}
		}
		if (sc.atLineEnd) {
			int lineState = lineStateLineType | (static_cast<int>(asmState) << 3)
				| (nestedLevel << 8);
			if (sc.state == SCE_D_DELIMITED_STRING) {
				lineState |= DLineStateMaskDelimitedString;
			}
			styler.SetLineState(sc.currentLine, lineState);
			lineStateLineType = 0;
			visibleChars = 0;
			visibleCharsBefore = 0;
			kwType = KeywordType::None;
			insideUrl = false;
		}
		sc.Forward();
	}

	sc.Complete();
}

struct FoldLineState {
	int lineComment;
	int packageImport;
	constexpr explicit FoldLineState(int lineState) noexcept:
		lineComment(lineState & DLineStateMaskLineComment),
		packageImport((lineState >> 1) & 1) {
	}
};

constexpr bool IsStreamCommentStyle(int style) noexcept {
	return style == SCE_D_COMMENTBLOCK
		|| style == SCE_D_COMMENTBLOCKDOC
		|| style == SCE_D_COMMENTDOCWORD
		|| style == SCE_D_COMMENTDOCMACRO
		|| style == SCE_D_TASKMARKER;
}

constexpr bool IsMultilineStringStyle(int style) noexcept {
	return style == SCE_D_STRING
		|| style == SCE_D_HEXSTRING
		|| style == SCE_D_RAWSTRING
		|| style == SCE_D_STRING_BT
		|| style == SCE_D_DELIMITED_STRING
		|| style == SCE_D_ESCAPECHAR
		|| style == SCE_D_DELIMITED_ID
		|| style == SCE_D_FORMAT_SPECIFIER;
}

void FoldDDoc(Sci_PositionU startPos, Sci_Position lengthDoc, int initStyle, LexerWordList, Accessor &styler) {
	const Sci_PositionU endPos = startPos + lengthDoc;
	Sci_Line lineCurrent = styler.GetLine(startPos);
	FoldLineState foldPrev(0);
	int levelCurrent = SC_FOLDLEVELBASE;
	if (lineCurrent > 0) {
		levelCurrent = styler.LevelAt(lineCurrent - 1) >> 16;
		foldPrev = FoldLineState(styler.GetLineState(lineCurrent - 1));
		const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent - 1, SCE_D_OPERATOR, SCE_D_TASKMARKER);
		if (bracePos) {
			startPos = bracePos + 1; // skip the brace
		}
	}

	int levelNext = levelCurrent;
	FoldLineState foldCurrent(styler.GetLineState(lineCurrent));
	Sci_PositionU lineStartNext = styler.LineStart(lineCurrent + 1);
	Sci_PositionU lineEndPos = sci::min(lineStartNext, endPos) - 1;

	char chNext = styler[startPos];
	int styleNext = styler.StyleAt(startPos);
	int style = initStyle;
	int visibleChars = 0;

	for (Sci_PositionU i = startPos; i < endPos; i++) {
		const char ch = chNext;
		chNext = styler.SafeGetCharAt(i + 1);
		const int stylePrev = style;
		style = styleNext;
		styleNext = styler.StyleAt(i + 1);

		switch (style) {
		case SCE_D_COMMENTBLOCKDOC:
		case SCE_D_COMMENTBLOCK:
			if (!IsStreamCommentStyle(stylePrev)) {
				levelNext++;
			} else if (!IsStreamCommentStyle(styleNext)) {
				levelNext--;
			}
			break;

		case SCE_D_NESTED_COMMENT:
		case SCE_D_NESTED_COMMENTDOC: {
			const int level = (ch == '/' && chNext == '+') ? 1 : ((ch == '+' && chNext == '/') ? -1 : 0);
			if (level != 0) {
				levelNext += level;
				i++;
				chNext = styler.SafeGetCharAt(i + 1);
				styleNext = styler.StyleAt(i + 1);
			}
		} break;

		case SCE_D_STRING:
		case SCE_D_HEXSTRING:
		case SCE_D_RAWSTRING:
		case SCE_D_STRING_BT:
		case SCE_D_DELIMITED_STRING:
			if (!IsMultilineStringStyle(stylePrev)) {
				levelNext++;
			} else if (!IsMultilineStringStyle(styleNext)) {
				levelNext--;
			}
			break;

		case SCE_D_OPERATOR:
			if (ch == '{' || ch == '[' || ch == '(') {
				levelNext++;
			} else if (ch == '}' || ch == ']' || ch == ')') {
				levelNext--;
			}
			break;
		}

		if (visibleChars == 0 && !IsSpaceEquiv(style)) {
			++visibleChars;
		}
		if (i == lineEndPos) {
			const FoldLineState foldNext(styler.GetLineState(lineCurrent + 1));
			if (foldCurrent.lineComment) {
				levelNext += foldNext.lineComment - foldPrev.lineComment;
			} else if (foldCurrent.packageImport) {
				levelNext += foldNext.packageImport - foldPrev.packageImport;
			} else if (visibleChars) {
				const Sci_PositionU bracePos = CheckBraceOnNextLine(styler, lineCurrent, SCE_D_OPERATOR, SCE_D_TASKMARKER);
				if (bracePos) {
					levelNext++;
					i = bracePos; // skip the brace
					chNext = '\0';
				}
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
			foldPrev = foldCurrent;
			foldCurrent = foldNext;
			visibleChars = 0;
		}
	}
}

}

LexerModule lmD(SCLEX_D, ColouriseDDoc, "d", FoldDDoc);
