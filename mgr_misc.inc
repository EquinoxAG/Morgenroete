%ifndef _MORGENROETE_MISC_INC_
%define _MORGENROETE_MISC_INC_

%macro INCLUDE 1
	%ifndef MGR_SOURCE_FILE
		%xdefine MGR_SOURCE_FILE __FILE__
	%endif
	%include %1
%endmacro

%endif
