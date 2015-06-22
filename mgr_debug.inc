%ifndef _MORGENROETE_DEBUG_INC_
%define _MORGENROETE_DEBUG_INC_


; 1 = macro name to debug
%macro DebugMacro 1
	%xdefine %1_DEBUG_MACRO
%endmacro

%macro UndebugMacro 1
	%undef %1_DEBUG_MACRO
%endmacro


;1 = name, rest will be added together and printed as error
%macro PrintIfDebugged 1-*
	%ifdef %1_DEBUG_MACRO
		%xdefine PIDGB_ErrorStr ''
		%rep (%0-1)
			%rotate 1
			%ifnstr %1
				%defstr PIDGBCurrStr %1
			%else
				%xdefine PIDGBCurrStr %1
			%endif
			%strcat PIDGB_ErrorStr PIDGB_ErrorStr, PIDGBCurrStr
		%endrep
		%rotate 1
		%undef PIDGBCurrStr
		%error Traced error in: %1  => PIDGB_ErrorStr
		%undef PIDGB_ErrorStr
	%endif
%endmacro 


%endif
