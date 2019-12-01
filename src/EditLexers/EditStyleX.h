#pragma once
#include "EditStyle.h"
// NP2StyleX_
#define	MULTI_STYLE(a, b, c, d)			((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))
#define EDITSTYLE_HOLE(style, name)		{ NP2STYLE_ ## style, CSTRLEN(name), (name), NULL }

#define EDITLEXER_HOLE(name, styles)	{ StyleTheme_Default, 0, 1, 1, COUNTOF(styles), CSTRLEN(name), (name), NULL, NULL }
#define EDITLEXER_TEXT(name, styles)	{ StyleTheme_Default, 0, 0, 0, COUNTOF(styles), CSTRLEN(name), (name), NULL, NULL }

#define NP2StyleX_MarginLineNumber		EDITSTYLE_HOLE(MarginLineNumber, L"Margin and Line Number")
#define NP2StyleX_MatchingBrace			EDITSTYLE_HOLE(MatchingBrace, L"Matching Brace")
#define NP2StyleX_MatchingBraceError	EDITSTYLE_HOLE(MatchingBraceError, L"Matching Brace Error")
#define NP2StyleX_ControlCharacter		EDITSTYLE_HOLE(ControlCharacter, L"Control Character")
#define NP2StyleX_IndentationGuide		EDITSTYLE_HOLE(IndentationGuide, L"Indentation Guide")
#define NP2StyleX_SelectedText			EDITSTYLE_HOLE(SelectedText, L"Selected Text")
#define NP2StyleX_Whitespace			EDITSTYLE_HOLE(Whitespace, L"Whitespace")
#define NP2StyleX_CurrentLine			EDITSTYLE_HOLE(CurrentLine, L"Current Line")
#define NP2StyleX_Caret					EDITSTYLE_HOLE(Caret, L"Caret")
#define NP2StyleX_LongLineMarker		EDITSTYLE_HOLE(LongLineMarker, L"Long Line Marker")
#define NP2StyleX_ExtraLineSpacing		EDITSTYLE_HOLE(ExtraLineSpacing, L"Extra Line Spacing")
#define NP2StyleX_FoldingMarker			EDITSTYLE_HOLE(FoldingMarker, L"Folding Marker")
#define NP2StyleX_MarkOccurrences		EDITSTYLE_HOLE(MarkOccurrences, L"Mark Occurrences")
#define NP2StyleX_DefaultCodeStyle		EDITSTYLE_HOLE(DefaultCodeStyle, L"Default Code Style")
#define NP2StyleX_DefaultTextStyle		EDITSTYLE_HOLE(DefaultTextStyle, L"Default Text Style")
#define NP2StyleX_FoldEllipsis			EDITSTYLE_HOLE(FoldEllipsis, L"Fold Ellipsis")
#define NP2StyleX_IMEIndicator			EDITSTYLE_HOLE(IMEIndicator, L"IME Indicator")
#define NP2StyleX_Bookmark				EDITSTYLE_HOLE(Bookmark, L"Bookmark")
#define NP2StyleX_CallTips				EDITSTYLE_HOLE(CallTips, L"CallTips")

#define NP2StyleX_Default				EDITSTYLE_HOLE(Default, L"Default")
#define NP2StyleX_Comment				EDITSTYLE_HOLE(Comment, L"Comment")
#define NP2StyleX_Keyword				EDITSTYLE_HOLE(Keyword, L"Keyword")
#define NP2StyleX_TypeKeyword			EDITSTYLE_HOLE(TypeKeyword, L"Type Keyword")
#define NP2StyleX_String				EDITSTYLE_HOLE(String, L"String")
#define NP2StyleX_Number				EDITSTYLE_HOLE(Number, L"Number")
#define NP2StyleX_Operator				EDITSTYLE_HOLE(Operator, L"Operator")
#define NP2StyleX_Preprocessor			EDITSTYLE_HOLE(Preprocessor, L"Preprocessor")
#define NP2StyleX_VerbatimString		EDITSTYLE_HOLE(VerbatimString, L"Verbatim String")
#define NP2StyleX_Regex					EDITSTYLE_HOLE(Regex, L"Regex")
#define NP2StyleX_Attribute				EDITSTYLE_HOLE(Attribute, L"Attribute")
#define NP2StyleX_Directive				EDITSTYLE_HOLE(Directive, L"Directive")

#define NP2StyleX_DocComment			EDITSTYLE_HOLE(DocComment, L"Doc Comment")
#define NP2StyleX_DocCommentTag			EDITSTYLE_HOLE(DocCommentTag, L"Doc Comment Tag")
#define NP2StyleX_XMLTag				EDITSTYLE_HOLE(XMLTag, L"XML Tag")
#define NP2StyleX_XMLAttribute			EDITSTYLE_HOLE(XMLAttribute, L"XML Attribute")
#define NP2StyleX_XMLValue				EDITSTYLE_HOLE(XMLValue, L"XML Value")
#define NP2StyleX_XMLString				EDITSTYLE_HOLE(XMLString, L"XML String")
#define NP2StyleX_XMLOtherTag			EDITSTYLE_HOLE(XMLOtherTag, L"XML Other Tag")
#define NP2StyleX_XMLComment			EDITSTYLE_HOLE(XMLComment, L"XML Comment")
#define NP2StyleX_XMLEntity				EDITSTYLE_HOLE(XMLEntity, L"XML Entity")
#define NP2StyleX_XMLElementText		EDITSTYLE_HOLE(XMLElementText, L"XML Element Text")
#define NP2StyleX_XMLIdentifier			EDITSTYLE_HOLE(XMLIdentifier, L"XML Identifier")
#define NP2StyleX_XMLSGML				EDITSTYLE_HOLE(XMLSGML, L"SGML")
#define NP2StyleX_XMLCDATA				EDITSTYLE_HOLE(XMLCDATA, L"CDATA")

#define NP2StyleX_HTMLTag				EDITSTYLE_HOLE(HTMLTag, L"HTML Tag")
#define NP2StyleX_HTMLUnknownTag		EDITSTYLE_HOLE(HTMLUnknownTag, L"HTML Unknown Tag")
#define NP2StyleX_HTMLAttribute			EDITSTYLE_HOLE(HTMLAttribute, L"HTML Attribute")
#define NP2StyleX_HTMLUnknownAttribute	EDITSTYLE_HOLE(HTMLUnknownAttribute, L"HTML Unknown Attribute")
#define NP2StyleX_HTMLValue				EDITSTYLE_HOLE(HTMLValue, L"HTML Value")
#define NP2StyleX_HTMLString			EDITSTYLE_HOLE(HTMLString, L"HTML String")
#define NP2StyleX_HTMLOtherTag			EDITSTYLE_HOLE(HTMLOtherTag, L"HTML Other Tag")
#define NP2StyleX_HTMLComment			EDITSTYLE_HOLE(HTMLComment, L"HTML Comment")
#define NP2StyleX_HTMLEntity			EDITSTYLE_HOLE(HTMLEntity, L"HTML Entity")
#define NP2StyleX_HTMLElementText		EDITSTYLE_HOLE(HTMLElementText, L"HTML Element Text")

#define NP2StyleX_ASPTag				EDITSTYLE_HOLE(ASPTag, L"ASP Tag")
#define NP2StyleX_PHPTag				EDITSTYLE_HOLE(PHPTag, L"PHP Tag")

#define NP2StyleX_PHPDefault			EDITSTYLE_HOLE(PHPDefault, L"PHP Default")
#define NP2StyleX_PHPComment			EDITSTYLE_HOLE(PHPComment, L"PHP Comment")
#define NP2StyleX_PHPKeyword			EDITSTYLE_HOLE(PHPKeyword, L"PHP Keyword")
#define NP2StyleX_PHPString				EDITSTYLE_HOLE(PHPString, L"PHP String")
#define NP2StyleX_PHPHeredocString		EDITSTYLE_HOLE(PHPHeredocString, L"PHP Heredoc String")
#define NP2StyleX_PHPNowdocString		EDITSTYLE_HOLE(PHPNowdocString, L"PHP Nowdoc String")
#define NP2StyleX_PHPNumber				EDITSTYLE_HOLE(PHPNumber, L"PHP Number")
#define NP2StyleX_PHPOperator			EDITSTYLE_HOLE(PHPOperator, L"PHP Operator")
#define NP2StyleX_PHPVariable			EDITSTYLE_HOLE(PHPVariable, L"PHP Variable")
#define NP2StyleX_PHPStringVariable		EDITSTYLE_HOLE(PHPStringVariable, L"PHP String Variable")
#define NP2StyleX_PHPComplexVariable	EDITSTYLE_HOLE(PHPComplexVariable, L"PHP Complex Variable")

#define NP2StyleX_JSDefault				EDITSTYLE_HOLE(JSDefault, L"JS Default")
#define NP2StyleX_JSComment				EDITSTYLE_HOLE(JSComment, L"JS Comment")
#define NP2StyleX_JSKeyword				EDITSTYLE_HOLE(JSKeyword, L"JS Keyword")

#define NP2StyleX_JSString				EDITSTYLE_HOLE(JSString, L"JS String")
#define NP2StyleX_JSRegex				EDITSTYLE_HOLE(JSRegex, L"JS Regex")
#define NP2StyleX_JSNumber				EDITSTYLE_HOLE(JSNumber, L"JS Number")
#define NP2StyleX_JSSymbol				EDITSTYLE_HOLE(JSSymbol, L"JS Symbol")

#define NP2StyleX_ASPJSDefault			EDITSTYLE_HOLE(ASPJSDefault, L"ASP JS Default")
#define NP2StyleX_ASPJSComment			EDITSTYLE_HOLE(ASPJSComment, L"ASP JS Comment")
#define NP2StyleX_ASPJSKeyword			EDITSTYLE_HOLE(ASPJSKeyword, L"ASP JS Keyword")

#define NP2StyleX_ASPJSString			EDITSTYLE_HOLE(ASPJSString, L"ASP JS String")
#define NP2StyleX_ASPJSRegex			EDITSTYLE_HOLE(ASPJSRegex, L"ASP JS Regex")
#define NP2StyleX_ASPJSNumber			EDITSTYLE_HOLE(ASPJSNumber, L"ASP JS Number")
#define NP2StyleX_ASPJSSymbol			EDITSTYLE_HOLE(ASPJSSymbol, L"ASP JS Symbol")

#define NP2StyleX_VBSDefault			EDITSTYLE_HOLE(VBSDefault, L"VBS Default")
#define NP2StyleX_VBSComment			EDITSTYLE_HOLE(VBSComment, L"VBS Comment")
#define NP2StyleX_VBSKeyword			EDITSTYLE_HOLE(VBSKeyword, L"VBS Keyword")

#define NP2StyleX_VBSString				EDITSTYLE_HOLE(VBSString, L"VBS String")
#define NP2StyleX_VBSNumber				EDITSTYLE_HOLE(VBSNumber, L"VBS Number")

#define NP2StyleX_ASPVBSDefault			EDITSTYLE_HOLE(ASPVBSDefault, L"ASP VBS Default")
#define NP2StyleX_ASPVBSComment			EDITSTYLE_HOLE(ASPVBSComment, L"ASP VBS Comment")
#define NP2StyleX_ASPVBSKeyword			EDITSTYLE_HOLE(ASPVBSKeyword, L"ASP VBS Keyword")

#define NP2StyleX_ASPVBSString			EDITSTYLE_HOLE(ASPVBSString, L"ASP VBS String")
#define NP2StyleX_ASPVBSNumber			EDITSTYLE_HOLE(ASPVBSNumber, L"ASP VBS Number")

#define NP2StyleX_TagClass				EDITSTYLE_HOLE(TagClass, L"Tag Class")
#define NP2StyleX_TagId					EDITSTYLE_HOLE(TagId, L"Tag Id")
#define NP2StyleX_TagAttribute			EDITSTYLE_HOLE(TagAttribute, L"Tag Attribute")
#define NP2StyleX_PseudoClassElement	EDITSTYLE_HOLE(PseudoClassElement, L"Pseudo Class/Element")
#define NP2StyleX_UnknownPseudoClass	EDITSTYLE_HOLE(UnknownPseudoClass, L"Unknown Pseudo Class")
#define NP2StyleX_CSSProperty			EDITSTYLE_HOLE(CSSProperty, L"CSS Property")
#define NP2StyleX_UnknownProperty		EDITSTYLE_HOLE(UnknownProperty, L"Unknown Property")
#define NP2StyleX_Media					EDITSTYLE_HOLE(Media, L"Media")
#define NP2StyleX_Value					EDITSTYLE_HOLE(Value, L"Value")
#define NP2StyleX_Important				EDITSTYLE_HOLE(Important, L"Important")

#define NP2StyleX_Class					EDITSTYLE_HOLE(Class, L"Class")
#define NP2StyleX_Struct				EDITSTYLE_HOLE(Struct, L"Struct")
#define NP2StyleX_Union					EDITSTYLE_HOLE(Union, L"Union")
#define NP2StyleX_Interface				EDITSTYLE_HOLE(Interface, L"Interface")
#define NP2StyleX_Trait					EDITSTYLE_HOLE(Trait, L"Trait")
#define NP2StyleX_Enumeration			EDITSTYLE_HOLE(Enumeration, L"Enumeration")
#define NP2StyleX_Constant				EDITSTYLE_HOLE(Constant, L"Constant")
#define NP2StyleX_Variable				EDITSTYLE_HOLE(Variable, L"Variable")
#define NP2StyleX_Function				EDITSTYLE_HOLE(Function, L"Function")
#define NP2StyleX_Method				EDITSTYLE_HOLE(Method, L"Method")
#define NP2StyleX_Field					EDITSTYLE_HOLE(Field, L"Field")
#define NP2StyleX_Property				EDITSTYLE_HOLE(Property, L"Property")
#define NP2StyleX_Macro					EDITSTYLE_HOLE(Macro, L"Macro")
#define NP2StyleX_Annotation			EDITSTYLE_HOLE(Annotation, L"Annotation")
#define NP2StyleX_Object				EDITSTYLE_HOLE(Object, L"Object")
#define NP2StyleX_Type					EDITSTYLE_HOLE(Type, L"Type")
#define NP2StyleX_FunctionDefine		EDITSTYLE_HOLE(FunctionDefine, L"Function Define")

#define NP2StyleX_Label					EDITSTYLE_HOLE(Label, L"Label")
#define NP2StyleX_Command				EDITSTYLE_HOLE(Command, L"Command")
#define NP2StyleX_RawString				EDITSTYLE_HOLE(RawString, L"Raw String")
#define NP2StyleX_HexString				EDITSTYLE_HOLE(HexString, L"Hex String")
#define NP2StyleX_Instruction			EDITSTYLE_HOLE(Instruction, L"Instruction")
#define NP2StyleX_Register				EDITSTYLE_HOLE(Register, L"Register")
#define NP2StyleX_Backticks				EDITSTYLE_HOLE(Backticks, L"Backticks")
#define NP2StyleX_TripleQuotedString	EDITSTYLE_HOLE(TripleQuotedString, L"Triple Quoted String")
#define NP2StyleX_BasicFunction			EDITSTYLE_HOLE(BasicFunction, L"Basic Function")
#define NP2StyleX_PredefinedVariable	EDITSTYLE_HOLE(PredefinedVariable, L"Predefined Variable")
#define NP2StyleX_DocCommentTagHTML		EDITSTYLE_HOLE(DocCommentTagHTML, L"Doc Comment HTML Tag")
#define NP2StyleX_ReservedWord			EDITSTYLE_HOLE(ReservedWord, L"Reserved Word")
#define NP2StyleX_EscapeSequence		EDITSTYLE_HOLE(EscapeSequence, L"Escape Sequence")

#define NP2StyleX_BracketArgument		EDITSTYLE_HOLE(BracketArgument, L"Bracket Argument")

#define NP2StyleX_LINQKeyword			EDITSTYLE_HOLE(LINQKeyword, L"LINQ Keyword")

#define NP2StyleX_ScalarVar				EDITSTYLE_HOLE(ScalarVar, L"Scalar $var")
#define NP2StyleX_ArrayVar				EDITSTYLE_HOLE(ArrayVar, L"Array @var")
#define NP2StyleX_HashVar				EDITSTYLE_HOLE(HashVar, L"Hash %var")
#define NP2StyleX_SymbolTableVar		EDITSTYLE_HOLE(SymbolTableVar, L"Symbol Table *var")
#define NP2StyleX_Regex_mre				EDITSTYLE_HOLE(Regex_mre, L"Regex /re/ or m{re}")
#define NP2StyleX_Substitution_re		EDITSTYLE_HOLE(Substitution_re, L"Substitution s/re/ore/")

#define NP2StyleX_HeredocDelimiter		EDITSTYLE_HOLE(HeredocDelimiter, L"Here-doc (Delimiter)")
#define NP2StyleX_HeredocSingleQuoted	EDITSTYLE_HOLE(HeredocSingleQuoted, L"Here-doc (Single Quoted, q)")
#define NP2StyleX_HeredocDoubleQuoted	EDITSTYLE_HOLE(HeredocDoubleQuoted, L"Here-doc (Double Quoted, qq)")
#define NP2StyleX_HeredocBackticks		EDITSTYLE_HOLE(HeredocBackticks, L"Here-doc (Backticks, qx)")
#define NP2StyleX_SingleQuotedString_q		EDITSTYLE_HOLE(SingleQuotedString_q, L"Single Quoted String (Generic, q)")
#define NP2StyleX_DoubleQuotedString_qq		EDITSTYLE_HOLE(DoubleQuotedString_qq, L"Double Quoted String (qq)")
#define NP2StyleX_Backticks_qx			EDITSTYLE_HOLE(Backticks_qx, L"Backticks (qx)")
#define NP2StyleX_Regex_qr				EDITSTYLE_HOLE(Regex_qr, L"Regex (qr)")
#define NP2StyleX_Array_qw				EDITSTYLE_HOLE(Array_qw, L"Array (qw)")
#define NP2StyleX_Prototype				EDITSTYLE_HOLE(Prototype, L"Prototype")
#define NP2StyleX_FormatIdentifier		EDITSTYLE_HOLE(FormatIdentifier, L"Format Identifier")
#define NP2StyleX_FormatBody			EDITSTYLE_HOLE(FormatBody, L"Format Body")
#define NP2StyleX_PODCommon				EDITSTYLE_HOLE(PODCommon, L"POD (Common)")
#define NP2StyleX_PODVerbatim			EDITSTYLE_HOLE(PODVerbatim, L"POD (Verbatim)")
#define NP2StyleX_DataSection			EDITSTYLE_HOLE(DataSection, L"Data Section")
#define NP2StyleX_ParsingError			EDITSTYLE_HOLE(ParsingError, L"Parsing Error")

#define NP2StyleX_Substitution			EDITSTYLE_HOLE(Substitution, L"Substitution")
#define NP2StyleX_Modifier				EDITSTYLE_HOLE(Modifier, L"Modifier")

#define NP2StyleX_StringTableMathFunc	EDITSTYLE_HOLE(StringTableMathFunc, L"String, Table & Math Function")
#define NP2StyleX_IOSystemFacility		EDITSTYLE_HOLE(IOSystemFacility, L"Input, Output & System Facility")

#define NP2StyleX_Bytes					EDITSTYLE_HOLE(Bytes, L"Bytes")
#define NP2StyleX_RawBytes				EDITSTYLE_HOLE(RawBytes, L"Raw Bytes")
#define NP2StyleX_TripleQuotedBytes		EDITSTYLE_HOLE(TripleQuotedBytes, L"Triple Quoted Bytes")
#define NP2StyleX_Lifetime				EDITSTYLE_HOLE(Lifetime, L"Lifetime")
#define NP2StyleX_Decorator				EDITSTYLE_HOLE(Decorator, L"Decorator")
#define NP2StyleX_BuildinFunction		EDITSTYLE_HOLE(BuildinFunction, L"Build-in Function")
#define NP2StyleX_objectMethod			EDITSTYLE_HOLE(objectMethod, L"object Method")
#define NP2StyleX_BuildinConstant		EDITSTYLE_HOLE(BuildinConstant, L"Build-in Constant")
#define NP2StyleX_FormattedString		EDITSTYLE_HOLE(FormattedString, L"Formatted String")
#define NP2StyleX_TripleQuotedFString	EDITSTYLE_HOLE(TripleQuotedFString, L"Triple Quoted Formatted String")
#define NP2StyleX_Module				EDITSTYLE_HOLE(Module, L"Module")
#define NP2StyleX_POD					EDITSTYLE_HOLE(POD, L"POD")
#define NP2StyleX_Symbol				EDITSTYLE_HOLE(Symbol, L"Symbol")

#define NP2StyleX_Cmdlet				EDITSTYLE_HOLE(Cmdlet, L"Cmdlet")
#define NP2StyleX_Alias					EDITSTYLE_HOLE(Alias, L"Alias")

#define NP2StyleX_SendKey				EDITSTYLE_HOLE(SendKey, L"Send Key")
#define NP2StyleX_Special				EDITSTYLE_HOLE(Special, L"Special")

#define NP2StyleX_PascalKeyword			EDITSTYLE_HOLE(PascalKeyword, L"Pascal Keyword")

#define NP2StyleX_Parameter				EDITSTYLE_HOLE(Parameter, L"Parameter")
#define NP2StyleX_InlineExpansion		EDITSTYLE_HOLE(InlineExpansion, L"Inline Expansion")

#define NP2StyleX_Section				EDITSTYLE_HOLE(Section, L"Section")
#define NP2StyleX_Assignment			EDITSTYLE_HOLE(Assignment, L"Assignment")
#define NP2StyleX_DefaultValue			EDITSTYLE_HOLE(DefaultValue, L"Default Value")

#define NP2StyleX_DoubleQuotedString	EDITSTYLE_HOLE(DoubleQuotedString, L"Double Quoted String")
#define NP2StyleX_SingleQuotedString	EDITSTYLE_HOLE(SingleQuotedString, L"Single Quoted String")
#define NP2StyleX_Key					EDITSTYLE_HOLE(Key, L"Key")
#define NP2StyleX_ParameterExpansion	EDITSTYLE_HOLE(ParameterExpansion, L"Parameter Expansion")
#define NP2StyleX_Table					EDITSTYLE_HOLE(Table, L"Table")

#define NP2StyleX_SourceDestination		EDITSTYLE_HOLE(SourceDestination, L"Source and Destination")
#define NP2StyleX_PositionSetting		EDITSTYLE_HOLE(PositionSetting, L"Position Setting")
#define NP2StyleX_LineAddition			EDITSTYLE_HOLE(LineAddition, L"Line Addition")
#define NP2StyleX_LineRemoval			EDITSTYLE_HOLE(LineRemoval, L"Line Removal")
#define NP2StyleX_LineChange			EDITSTYLE_HOLE(LineChange, L"Line Change")

#define NP2StyleX_CPUInstruction		EDITSTYLE_HOLE(CPUInstruction, L"CPU Instruction")
#define NP2StyleX_FPUInstruction		EDITSTYLE_HOLE(FPUInstruction, L"FPU Instruction")
#define NP2StyleX_ExtendedInstruction	EDITSTYLE_HOLE(ExtendedInstruction, L"Extended Instruction")
#define NP2StyleX_DirectiveOperand		EDITSTYLE_HOLE(DirectiveOperand, L"Directive Operand")

#define NP2StyleX_MagicMethod			EDITSTYLE_HOLE(MagicMethod, L"Magic Method")
#define NP2StyleX_MagicConstant			EDITSTYLE_HOLE(MagicConstant, L"Magic Constant")
#define NP2StyleX_TemplateLiteral		EDITSTYLE_HOLE(TemplateLiteral, L"Template Literal")
#define NP2StyleX_HeredocString			EDITSTYLE_HOLE(HeredocString, L"Heredoc String")
#define NP2StyleX_NowdocString			EDITSTYLE_HOLE(NowdocString, L"Nowdoc String")
#define NP2StyleX_DateTime				EDITSTYLE_HOLE(DateTime, L"DateTime")
#define NP2StyleX_Action				EDITSTYLE_HOLE(Action, L"Action")
#define NP2StyleX_ModuleDirective		EDITSTYLE_HOLE(ModuleDirective, L"Module Directive")
#define NP2StyleX_Reference				EDITSTYLE_HOLE(Reference, L"Reference")
#define NP2StyleX_TextBlock				EDITSTYLE_HOLE(TextBlock, L"Text Block")
#define NP2StyleX_Math					EDITSTYLE_HOLE(Math, L"Math")
#define NP2StyleX_SpecialCharacter		EDITSTYLE_HOLE(SpecialCharacter, L"Special Character")
#define NP2StyleX_Tag					EDITSTYLE_HOLE(Tag, L"Tag")
#define NP2StyleX_Option				EDITSTYLE_HOLE(Option, L"Option")
#define NP2StyleX_Title					EDITSTYLE_HOLE(Title, L"Title")
#define NP2StyleX_Quote					EDITSTYLE_HOLE(Quote, L"Quote")
#define NP2StyleX_VerbatimSegment		EDITSTYLE_HOLE(VerbatimSegment, L"Verbatim Segment")
#define NP2StyleX_ListCode				EDITSTYLE_HOLE(ListCode, L"List Code")
#define NP2StyleX_Chapter				EDITSTYLE_HOLE(Chapter, L"Chapter")
#define NP2StyleX_Document				EDITSTYLE_HOLE(Document, L"Document")
#define NP2StyleX_StdPackage			EDITSTYLE_HOLE(StdPackage, L"STD Package")
#define NP2StyleX_StdOperator			EDITSTYLE_HOLE(StdOperator, L"STD Operator")
#define NP2StyleX_StdFunction			EDITSTYLE_HOLE(StdFunction, L"STD Function")
#define NP2StyleX_StdType				EDITSTYLE_HOLE(StdType, L"STD Type")
#define NP2StyleX_UserDefine			EDITSTYLE_HOLE(UserDefine, L"User Define")
#define NP2StyleX_SystemTaskFunction	EDITSTYLE_HOLE(SystemTaskFunction, L"System Task and Function")
#define NP2StyleX_Intrinsic				EDITSTYLE_HOLE(Intrinsic, L"Intrinsic")
#define NP2StyleX_Metadata				EDITSTYLE_HOLE(Metadata, L"Metadata")
#define NP2StyleX_COMDAT				EDITSTYLE_HOLE(COMDAT, L"COMDAT")
#define NP2StyleX_GlobalVariable		EDITSTYLE_HOLE(GlobalVariable, L"Global Variable")
#define NP2StyleX_DelimitedString		EDITSTYLE_HOLE(DelimitedString, L"Delimited String")
#define NP2StyleX_TokenString			EDITSTYLE_HOLE(TokenString, L"Token String")
#define NP2StyleX_CodeQuotation			EDITSTYLE_HOLE(CodeQuotation, L"Code Quotation")
#define NP2StyleX_InlineAsm				EDITSTYLE_HOLE(InlineAsm, L"Inline Asm")
#define NP2StyleX_Callback				EDITSTYLE_HOLE(Callback, L"Callback")
#define NP2StyleX_Target				EDITSTYLE_HOLE(Target, L"Target")
#define NP2StyleX_BuildinRule			EDITSTYLE_HOLE(BuildinRule, L"Build-in Rule")
#define NP2StyleX_BlobHex				EDITSTYLE_HOLE(BlobHex, L"Blob Hex")
#define NP2StyleX_BitField				EDITSTYLE_HOLE(BitField, L"Bit Field")

#define EDITSTYLE_DEFAULT 				{ STYLE_DEFAULT, NP2StyleX_Default, L"" }
