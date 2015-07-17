%ifndef _MORGENROETE_INSTRUCTIONS_INC_
%define _MORGENROETE_INSTRUCTIONS_INC_

%include "mgr_interface.inc"
%include "mgr_stack.inc"
%include "mgr_debug.inc"

%macro mov_tus 2
	CheckUpdateStackPtr
	SearchReplaceLocalVar %1, OP0, OP0_STRUC
	SearchReplaceLocalVar %2, OP1, OP1_STRUC

	mov OP0, OP1
	PrintIfDebugged %??, 'mov ', OP0,',', OP1
	
	%undef OP0
	%undef OP1
	%undef OP1_STRUC
	%undef OP0_STRUC
%endmacro

%macro mov_ts 2
	CheckUpdateStackPtr
	SearchReplaceLocalVar %1, OP0, OP0_STRUC
	SearchReplaceLocalVar %2, OP1, OP1_STRUC

	%defstr OP1_STR OP1
	%defstr OP0_STR OP0

	%ifndef OP0_STRUC
		%ifndef OP1_STRUC
			mov OP0, OP1	;No known object just move the data
			PrintIfDebugged %??, 'mov ', OP0,',', OP1
		%else
			%xdefine OP1_MEM_SIZE OP1_STRUC %+ _size
			;OP1_STRUC is known but OP0_STRUC is unknown
			DeductType OP1_SIZE, OP1_STR	;Trying to deduct the access size
			%ifdef OP1_SIZE	;could deduct access size
				%ifn OP1_SIZE = 177		; If the access size is not unknwon
					%if OP1_SIZE > OP1_MEM_SIZE
						%error Trying to access OP1_SIZE bytes of the OP1_MEM_SIZE bytes width structure OP1_STRUC
						%error Instruction fault : mov OP0, OP1
					%else
						mov OP0, OP1	;Access is okay
						PrintIfDebugged %??, 'mov ', OP0,',', OP1
					%endif
				%else	; Mem access depends on OP0
					DeductType OP0_SIZE, OP0_STR
					%ifndef OP0_SIZE
						;Could not deduct access size just access it and let do nasm its work
						mov OP0, OP1
						PrintIfDebugged %??, 'mov ', OP0,',', OP1
					%else
						%if OP0_SIZE > OP1_MEM_SIZE
							%error Trying to access OP0_SIZE bytes of the OP1_MEM_SIZE bytes width structure OP1_STRUC
							%error Instruction fault : mov OP0, OP1
						%else
							mov OP0, OP1
							PrintIfDebugged %??, 'mov ', OP0,',', OP1
						%endif
					%endif
				%endif
			%else
				;Load effective address requested
				%macro ReverseLoop 1-*
					%xdefine HAS_UPDATED 0
					%rep %0
						SearchAndReplace NEW_OFFSET, OP1_STR, %1,'0', NEW_THANG
						%if NEW_THANG > 0
							%deftok INSTI %1
							mov OP0, INSTI
							%deftok NEW_OFF NEW_OFFSET
							%assign NEW_OFF NEW_OFF

							%ifn NEW_OFF = 0
								%if NEW_OFF < 0
									%assign NEW_OFF -NEW_OFF
									sub OP0, NEW_OFF
								%else
									add OP0, NEW_OFF
								%endif
							%endif

							%undef NEW_OFF
							%undef INSTI
							%xdefine HAS_UPDATED (HAS_UPDATED+1)
						%endif
						%undef NEW_THANG
						%undef NEW_OFFSET
						%rotate 1
					%endrep
					%ifndef HAS_UPDATED
						mov OP0, OP1
					%else
						%if HAS_UPDATED > 1
							%error The effective address is too complex to compile! HAS_UPDATED
						%endif
						%undef HAS_UPDATED
					%endif
				%endmacro
				ReverseLoop 'rax', 'rbx', 'rcx', 'rdx', 'rsi', 'rdi', 'r8', 'r9','r10','r11','r12','r13','r14','r15', 'rbp', 'rsp'
				%unmacro ReverseLoop 1-*
			%endif
			
			%undef OP1_SIZE
			%undef OP1_MEM_SIZE
			%undef OP0_SIZE	
		%endif
	%else		;OP0_STRUC is defined => OP1 must be a register or a number
		%ifndef OP1_STRUC	;OP1_STRUC is undefined means, type deducting is the way to choose
			%xdefine OP0_MEM_SIZE OP0_STRUC %+ _size
			
			DeductType OP0_SIZE, OP0_STR

			%ifdef OP0_SIZE
				%ifn OP0_SIZE = 177
					%if OP0_SIZE > OP0_MEM_SIZE
						%error Trying to access OP0_SIZE bytes of the OP0_MEM_SIZE bytes width structure OP0_STRUC
						%error Instruction fault : mov OP0, OP1
					%else
						mov OP0, OP1
						PrintIfDebugged %??, 'mov ', OP0,',', OP1
					%endif
				%else
			
					DeductType OP1_SIZE, OP1_STR	;
					%ifdef OP1_SIZE
						%if OP1_SIZE > OP0_MEM_SIZE
							%error Trying to access OP1_SIZE bytes of the OP0_MEM_SIZE bytes width structure OP0_STRUC
							%error Instruction fault : mov OP0, OP1
						%else
							mov OP0, OP1
							PrintIfDebugged %??, 'mov ', OP0,',', OP1
						%endif
					%else
						;Unknown case try to move data
						mov OP0, OP1
						PrintIfDebugged %??, 'mov ', OP0,',', OP1
					%endif
				%endif

			%else
				;Unknown test try to move anyways
				mov OP0, OP1
				PrintIfDebugged %??, 'mov ', OP0,',', OP1
			%endif
		%endif
		%undef OP0_MEM_SIZE
		%undef OP0_SIZE
		%undef OP1_SIZE

	%endif
		
	%undef OP0
	%undef OP1
	%undef OP0_STRUC
	%undef OP1_STRUC
	%undef OP0_STR
	%undef OP1_STR
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
			%substr FARLV_tmp_str UnderlyingStr (FARLV_counter_end+1)
			%if FARLV_tmp_str = '>'
				%undef FARLV_SUSPECT
			%endif
			%undef FARLV_tmp_str
		%elif FARLV_counter_end = FARLV_CurrStrlen
			%xdefine FARLV_SUSPECT
			%assign FARLV_counter_end (FARLV_counter_end+1)
		%endif

		%ifdef FARLV_SUSPECT	
			%undef FARLV_SUSPECT
			%substr FARLV_CurrStr UnderlyingStr FARLV_counter, (FARLV_counter_end-FARLV_counter)

			CutAllCharactersOfType FARLV_TmpStr, FARLV_CurrStr, ')'
			%undef FARLV_CurrStr
			CutAllCharactersOfType FARLV_CurrStr, FARLV_TmpStr, '('
			%undef FARLV_TmpStr
	
			PartStringsByChar FARLV_CurrStr, '.', FARLV_STR0, FARLV_STR1
			PartStringsByChar FARLV_STR0, '->', FARLV_InitialOffset, FARLV_InitialStructure

			%ifdef FARLV_InitialStructure
				%deftok FARLV_IS_CONNECTED FARLV_InitialStructure

				%undef FARLV_IsStck
				IsStackVar FARLV_IsStck, FARLV_InitialOffset

				%ifdef FARLV_IsStck
					%defstr FARLV_InitialOffset FARLV_IsStck
					%strcat FARLV_InitialOffset '(rbp-', FARLV_InitialOffst
					%strcat FARLV_InitialOffset FARLV_InitialOffset, ')'
					%undef FARLV_IsStck
				%endif
				%xdefine FARLV_STR0 FARLV_InitialOffset
				%undef FARLV_InitialStructure
				%undef FARLV_InitialOffset
			%else
				%undef FARLV_InitialOffset
				%undef FARLV_IsStck
				IsStackVar FARLV_IsStck, FARLV_STR0
				%ifdef FARLV_IsStck
					GetStrucFromStackVar FARLV_IS_CONNECTED, FARLV_STR0
					%defstr FARLV_STR0 FARLV_IsStck
					%strcat FARLV_STR0 '(rbp-', FARLV_STR0
					%strcat FARLV_STR0 FARLV_STR0,')'
					%undef FARLV_IsStck
				%else
					IsConnectedToHeap FARLV_STR0, FARLV_IS_CONNECTED
					%ifndef FARLV_IS_CONNECTED
						IsInterface FARLV_STR0, FARLV_IS_CONNECTED
						%ifdef FARLV_IS_CONNECTED
							%deftok FARLV_IS_CONNECTED FARLV_STR0
							%xdefine FARLV_STR0 '0'
						%endif
					%endif
				%endif
			%endif

			%ifdef FARLV_IS_CONNECTED
				%strcat FARLV_endStr FARLV_endStr,'('
				%strcat FARLV_endStr FARLV_endStr, FARLV_STR0
				%ifdef FARLV_STR1
					%strcat FARLV_endStr FARLV_endStr,'+'
					TraverseInterfaceChain FARLV_STR1, FARLV_IS_CONNECTED, FARLV_offset, FARLV_struc
					%strcat FARLV_endStr FARLV_endStr, FARLV_offset
					%undef FARLV_offset
					%xdefine %3 FARLV_struc
					%undef FARLV_STR1
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

				%undef FARLV_StrStart
				%undef FARLV_StrEnd
			%else
			
			%endif

			%undef FARLV_InitialStructure
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
