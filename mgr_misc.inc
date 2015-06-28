%ifndef _MORGENROETE_MISC_INC_
%define _MORGENROETE_MISC_INC_

%include "mbr_string.inc"

%macro INCLUDE 1
	%ifndef MGR_SOURCE_FILE
		%xdefine MGR_SOURCE_FILE __FILE__
	%endif
	%include %1
%endmacro

;1 = file name, 2 = macro name
%macro ExportSymbolIfInFile 2
	%ifdef MGR_SOURCE_FILE
		PartStringsByCharLast MGR_SOURCE_FILE, '/', STR_WEAK, STR_STRONG
	%else
		PartStringsByCharLast __FILE__, '/', STR_WEAK, STR_STRONG
	%endif
	%undef STR_WEAK
	MGRStrcmp %3, STR_STRONG, dc_isequal
	%ifdef dc_isequal
		%undef dc_isequal
	%else
		extern %2
	%endif
	%undef STR_STRONG
%endmacro

%endif
