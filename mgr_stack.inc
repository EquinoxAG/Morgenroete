%ifndef _MORGENROETE_STACK_INC_
%define _MORGENROETE_STACK_INC_

%include "mgr_stack.inc"

%assign NUM_STACKS_OPEN 0

%macro CreateStack 1
	%ifdef STACK_NAME_%1
		%error A Stack with this name does already exist
	%else
		%xdefine STACK_NAME_%1 %1
		%xdefine STACK_SPACE_%1_CURRENT 0
		%xdefine STACK_SPACE_%1_FLOATING 0
		%push %1
		push rbp
		mov rbp, rsp
		%xdefine %$STACK_NAME %1
		%xdefine %$CONSTRUCTOR_LIST ''
		%xdefine %$DESTRUCTOR_LIST ''
		%assign NUM_STACKS_OPEN (NUM_STACKS_OPEN+1)
	%endif
%endmacro

;1 = name of the variable, 2 = interface of the variable
%macro ReserveStackSpace 2
	InternReserveStackSpace %$STACK_NAME, %1, %2
%endmacro

%macro ReserveStackSpace 3+
	%defstr RSSArgList %3
	InternReserveStackSpace %$STACK_NAME, %1,%2, RSSArgList
	%undef RSSArgList
%endmacro

%macro InternReserveStackSpace 3-4
	%ifdef STACK_NAME_%1
		%defstr IRSS_TryStr %2
		IsStackVar IRSS_IsVar, IRSS_TryStr
		%undef IRSS_TryStr

		%ifdef IRSS_IsVar
			%undef IRSS_IsVar
			%error The variable %2 is already defined!	
		%else
			%xdefine STACK_SPACE_%1_FLOATING (STACK_SPACE_%1_FLOATING+%3_size)
			%xdefine %$%2_VAR STACK_SPACE_%1_FLOATING
			%xdefine %$%2_STRUC %3

			%ifdef %3_destructor
				%strlen IRSS_DstLength %$DESTRUCTOR_LIST
				%ifn IRSS_DstLength = 0
					%strcat %$DESTRUCTOR_LIST %$DESTRUCTOR_LIST,','	
				%endif
				%defstr IRSS_Appending %2
				%strcat IRSS_Appending '%$',IRSS_Appending
				%strcat %$DESTRUCTOR_LIST %$DESTRUCTOR_LIST, IRSS_Appending
				%undef IRSS_Appending
				%xdefine %$%2_DESTRUCTOR %3_destructor
			%endif
		%endif
	%else
		%error Unknown error occured
	%endif
%endmacro

;1 = stack name
%macro CallDestructors 0
	%strlen CD_DstStrlen %$DESTRUCTOR_LIST
	%ifn CD_DstStrlen = 0
		push rbx
		%deftok CD_ArgList %$DESTRUCTOR_LIST
		CallDst CD_ArgList
		%undef CD_ArgList
		pop rbx
	%endif
	%undef CD_DstStrlen
%endmacro

%macro CallDst 1-*
	%defstr CD_Var %1
	%xdefine CD_DestFunc CD_Var
	%strcat CD_Var CD_Var, '_VAR'
	%strcat CD_DestFunc CD_DestFunc, '_DESTRUCTOR'
	%deftok CD_Var CD_Var
	%deftok CD_DestFunc CD_DestFunc

	mov rbx, rbp
	sub rbx, CD_Var

	secure_call CD_DestFunc, rbx

	%undef CD_Var
	%undef CD_DestFunc
%endmacro

;1 = to define structure, 2 = string of the var
%macro GetStrucFromStackVar 2
	%xdefine %1 '%$'
	%strcat %1 %1, %2
	%strcat %1 %1, '_STRUC'
	%deftok %1 %1
%endmacro
;1 = to define if stack var, 2 = name of the variable
%macro IsStackVar 2
	%strcat ISV_Investigate '%$',%2
	%strcat ISV_Investigate ISV_Investigate, '_VAR'
	%deftok ISV_InvestigateTok ISV_Investigate
	%defstr ISV_InvestigateAfterTok ISV_InvestigateTok

	MGRStrcmp ISV_InvestigateAfterTok, ISV_Investigate, ISV_IsEqual
	
	%ifdef ISV_IsEqual
		%undef ISV_IsEqual
		%undef %1
	%else
		%xdefine %1 ISV_InvestigateTok
	%endif

	%undef ISV_Investigate
	%undef ISV_InvestigateTok
	%undef ISV_InvestigateAfterTok
%endmacro


%macro CheckUpdateStackPtr 0
	%xdefine STACK_SPACE (STACK_SPACE_ %+ %$STACK_NAME %+ _CURRENT)
	%xdefine STACK_SPACE_TRAILING (STACK_SPACE_ %+ %$STACK_NAME %+ _FLOATING)
	%ifn STACK_SPACE = STACK_SPACE_TRAILING
		%warning There is stack space reserved in macros which is not reserved in the code yet. Use UpdateStackPtr to update the stack ptr inside the code
	%endif
	%undef STACK_SPACE
	%undef STACK_SPACE_TRAILING
%endmacro

%macro UpdateStackPtr 0
	%xdefine STACK_SPACE STACK_SPACE_ %+ %$STACK_NAME %+ _CURRENT
	%xdefine STACK_SPACE_TRAILING STACK_SPACE_ %+ %$STACK_NAME %+ _FLOATING
	%ifn STACK_SPACE = STACK_SPACE_TRAILING
		sub rsp, (STACK_SPACE_TRAILING-STACK_SPACE)
		%macro USP_DeepDefine 2
			%xdefine STACK_SPACE_%1_CURRENT %2
		%endmacro
		USP_DeepDefine %$STACK_NAME, STACK_SPACE_TRAILING
		%unmacro USP_DeepDefine 2
	%endif
	%undef STACK_SPACE
	%undef STACK_SPACE_TRAILING
%endmacro

%macro DestroyStack 1
	%ifdef STACK_NAME_%1
		CallDestructors
		%assign NUM_STACKS_OPEN (NUM_STACKS_OPEN-1)
		%pop %1
		mov rsp, rbp
		pop rbp	
		%undef STACK_NAME_%1
		%undef STACK_SPACE_%1_CURRENT
		%undef STACK_SPACE_%1_FLOATING
	%else
		%error A Stack with this name does not exist
	%endif
%endmacro




%endif
