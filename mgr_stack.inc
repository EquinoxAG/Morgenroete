%ifndef _MORGENROETE_STACK_INC_
%define _MORGENROETE_STACK_INC_

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

%macro CheckUpdateStackPtr 0
	%xdefine STACK_SPACE STACK_SPACE_ %+ %$STACK_NAME %+ _CURRENT
	%xdefine STACK_SPACE_TRAILING STACK_SPACE_ %+ %$STACK_NAME %+ _FLOATING
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
