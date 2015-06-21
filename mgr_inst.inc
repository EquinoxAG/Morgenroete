%ifndef _MORGENROETE_INSTRUCTIONS_INC_
%define _MORGENROETE_INSTRUCTIONS_INC_

%include "mgr_interface.inc"
%include "mgr_stack.inc"


%macro mov_ts 2
	SearchReplaceLocalVar %1, OP0, OP0_STRUC
	SearchReplaceLocalVar %2, OP1, OP1_STRUC


	%undef OP0
	%undef OP1
	%undef OP0_STRUC
	%undef OP1_STRUC
%endmacro


;1 = token to change, 2 = new token to define, 3 = new token objet size
%macro SearchReplaceLocalVar 3
	%defstr FARLV_KlopStr %1
	CutAllWhiteSpace UnderlyingStr, FARLV_KlopStr
	%undef FARLV_KlopStr

	%assign FARLV_counter 1
	%assign FARLV_counter_end 1
	%strlen FARLV_strlen UnderlyingStr
	%xdefine FARLV_endStr ''
	%assign FARLV_CurrStrlen FARLV_strlen

	%rep FARLV_strlen
		%substr FARLV_character UnderlyingStr FARLV_counter_end
		%if FARLV_character = '['
			%xdefine FARLV_SUSPECT
		%elif FARLV_character = ']'
			%xdefine FARLV_SUSPECT
		%elif FARLV_character = '+'
			%xdefine FARLV_SUSPECT
		%elif FARLV_character = '-'
			%xdefine FARLV_SUSPECT
		%elif FARLV_counter_end = FARLV_CurrStrlen
			%xdefine FARLV_SUSPECT
			%assign FARLV_counter_end (FARLV_counter_end+1)
		%endif

		%ifdef FARLV_SUSPECT	
			%undef FARLV_SUSPECT
			%substr FARLV_CurrStr UnderlyingStr FARLV_counter, (FARLV_counter_end-FARLV_counter)
			
			PartStringsByChar FARLV_CurrStr, '.', FARLV_STR0, FARLV_STR1
			IsConnectedToHeap FARLV_STR0, FARLV_IS_CONNECTED

			%ifdef FARLV_IS_CONNECTED
				%strcat FARLV_endStr FARLV_endStr,'('
				%strcat FARLV_endStr FARLV_endStr, FARLV_STR0
				%ifdef FARLV_STR1
					%strcat FARLV_endStr FARLV_endStr,'+'
					TraverseInterfaceChain FARLV_STR1, FARLV_IS_CONNECTED, FARLV_offset, FARLV_struc
					%strcat FARLV_endStr FARLV_endStr, FARLV_offset
					%undef FARLV_offset
					%xdefine %3 FARLV_struc
				%else
					%xdefine %3 FARLV_IS_CONNECTED
				%endif
				
				%undef FARLV_struc
				%strcat FARLV_endStr FARLV_endStr, ')'
				%undef FARLV_IS_CONNECTED
				
				%substr FARLV_StrStart UnderlyingStr 1, (FARLV_counter-1)
				%substr FARLV_StrEnd UnderlyingStr FARLV_counter_end, ((FARLV_CurrStrlen+1)-FARLV_counter_end)
				%xdefine UnderlyingStr FARLV_StrStart
				%strcat UnderlyingStr UnderlyingStr, FARLV_endStr
				%strlen FARLV_counter UnderlyingStr
				%assign FARLV_counter (FARLV_counter+1)
				%xdefine FARLV_counter_end FARLV_counter
				%xdefine FARLV_endStr ''
				%strcat UnderlyingStr UnderlyingStr, FARLV_StrEnd
				%strlen FARLV_CurrStrlen UnderlyingStr
				%error UnderlyingStr

				%undef FARLV_StrStart
				%undef FARLV_StrEnd
			%else
				%error FARLV_STR0 is not connected to heap
			%endif

			%undef FARLV_STR0
			%undef FARLV_STR1
			%assign FARLV_counter (FARLV_counter_end+1)
		%endif

		%assign FARLV_counter_end (FARLV_counter_end+1)
	%endrep

	%deftok %2 UnderlyingStr
	%undef FARLV_endStr
	%undef FARLV_counter
	%undef FARLV_counter_end
	%undef FARLV_CurrStr
	%undef FARLV_CurrStrlen
	%undef UnderlyingStr
	%undef FARLV_strlen
%endmacro




%endif
