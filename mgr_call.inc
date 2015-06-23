%ifndef _MORGENROETE_CALLING_CONVENTIONS_INC_
%define _MORGENROETE_CALLING_CONVENTIONS_INC_

%include "mgr_debug.inc"
%include "mgr_string.inc"
%include "mgr_inst.inc"
%include "mgr_stack.inc"

%macro StdExtCall 1
	force_call %1
%endmacro

%define STDCALL_GCC64 'STDCALL_VX64GCC'
;1 = function name, 2 = parameter count, 3 = from which file, 4 = calling convention
%macro DefineCall 3-4 'STDCALL_VX64GCC'
	%xdefine %1_parameter_count %2
	%xdefine %1_calling_convention %4
	%xdefine %1_call StdExtCall
	%undef %1_parameter_count_var

	%pathsearch FullFileName %3

	MGRStrcmp FullFileName, __FILE__, DC_IsEqual
	%ifdef DC_IsEqual
		%undef DC_IsEqual
	%else
		extern %1
	%endif
%endmacro

;1 = function name, 2 = parameter count, 3 = calling convention
%macro DeclareCall 2
	%ifdef LAST_DECLARED_FUNCTION
		%error Before declaring a new call, close the last call with EndFunction first!
	%else
		%ifdef %1_parameter_count
			%ifn %1_parameter_count = %2
				%error The parameter count differs in declaration and definition, in function %1
			%else
				global %1
				%1:
					MGRStrcmp %1_calling_convention, 'STDCALL_VX64GCC', DC_IsEqual
					%ifndef DC_IsEqual
					%endif
					%undef DC_IsEqual
					CreateStack %1_STACK
					DefineArguments %1_parameter_count
					%xdefine LAST_DECLARED_FUNCTION %1_STACK
			
			%endif
		%endif
	%endif
%endmacro

;1 = name
%macro EndFunction 0
	DestroyStack LAST_DECLARED_FUNCTION
	UndefineArguments
	%undef LAST_DECLARED_FUNCTION
	ret
%endmacro

%macro DefineVariableCall 3-4 'STDCALL_VX64GCC'
	%xdefine %1_parameter_count %2
	%xdefine %1_calling_convention %4
	%xdefine %1_call StdExtCall
	%xdefine %1_parameter_count_var

	%pathsearch FullFileName %3

	MGRStrcmp FullFileName, __FILE__, DC_IsEqual
	%ifdef DC_IsEqual
		%undef DC_IsEqual
	%else
		extern %1
	%endif
%endmacro


;1 = function name, 2 = parameter count,4 = macro to call the function takes 1 parameter, 5 = calling convention
%macro DefinePolymorphicCall 4-5 'STDCALL_VX64GCC'
	%xdefine %1_parameter_count %2
	%xdefine %1_calling_convention %5
	%xdefine %1_call %4
%endmacro


%macro secure_call 1-2+
	%ifndef %1_parameter_count 
		%warning The function was not defined by DefineCall therefore secure_call can not ensure a secure call
	%else
		%deftok CALL_CONV %1_calling_convention
		%if %0 = 1
			CALL_CONV %1
		%else
			CALL_CONV %1, %2
		%endif
		%undef CALL_CONV
	%endif	
%endmacro	

%macro move_if_unequal 2
	%defstr MIU_STR0 %1
	%defstr MIU_STR1 %2

	MGRStrcmp MIU_STR0, MIU_STR1, MIU_IsEqual
	
	%ifdef MIU_IsEqual
		%undef MIU_IsEqual
	%else
		mov_ts %1, %2
	%endif

	%undef MIU_STR0
	%undef MIU_STR1

%endmacro

%macro DefineArguments 1
		%if %1 > 0
			%xdefine Argument_0 rdi
			%if %1 > 1
				%xdefine Argument_1 rsi
				%if %1 > 2
					%xdefine Argument_2 rdx
					%if %1 > 3
						%xdefine Argument_3 rcx
						%if %1 > 4
							%xdefine Argument_4 r8
							%if %1 > 5
								%xdefine Argument_5 r9
								%if %1 > 6
									%assign DA_counter 6
									%rep (%0-6)
										DivideStringsByChar_IDD Argument_, DA_counter, rbp + ( 8*(DA_counter-6) )
										%assign DA_counter (DA_counter+1)
									%endrep
									%undef DA_counter
								%endif
							%endif
						%endif
					%endif
				%endif
			%endif
		%endif
%endmacro

%macro UndefineArguments 0
	%undef Argument_0
	%undef Argument_1
	%undef Argument_2
	%undef Argument_3
	%undef Argument_4
	%undef Argument_5
	%undef Argument_6
	%undef Argument_7
	%undef Argument_8
	%undef Argument_9
	%undef Argument_10
%endmacro

%macro STDCALL_UEFI 1-*
	%xdefine CALL_FUNC %1_call
	%xdefine FUNC_NAME %1

	%ifdef %1_parameter_count_var
		%if %1_parameter_count > (%0-1)
			%xdefine STD_PAR_ERROR
		%endif
	%else
		%ifn %1_parameter_count = (%0-1)
			%xdefine STD_PAR_ERROR
		%endif
	%endif		

	
	%ifndef STD_PAR_ERROR
		%if %0 > 1
			move_if_unequal rcx, %2
			%if %0 > 2
				move_if_unequal rdx, %3
				%if %0 > 3
					move_if_unequal r8, %4
					%if %0 > 4
						move_if_unequal r9, %5
						%if %0 > 5
							%rep (%0-5)	
								%rotate -1
								push %1
							%endrep
						%endif
					%endif
				%endif
			%endif
		%endif	
		sub rsp, 32			; Reserve Shadow Space	
		CALL_FUNC FUNC_NAME
		%if %0 > 5
			add rsp, (32 + ((%0-5)*8))
		%else
			add rsp, 32
		%endif
	%else
		%undef STD_PAR_ERROR
		%assign SD_Par (%0-1)
		%ifdef %1_parameter_count_var
			%error The function %1 takes a minimum parameter count of %1_parameter_count instead of SD_Par parameters
		%else
			%error The function %1 takes %1_parameter_count parameters instead of SD_Par parameters
		%endif
		%undef SD_Par
	%endif

	%undef CALL_FUNC
	%undef FUNC_NAME
%endmacro

%macro STDCALL_VX64GCC 1-*
	%xdefine CALL_FUNC %1_call	
	%xdefine FUNC_NAME %1

	%ifdef %1_parameter_count_var
		%if %1_parameter_count > (%0-1)
			%xdefine STD_PAR_ERROR
		%endif
	%else
		%ifn %1_parameter_count = (%0-1)
			%xdefine STD_PAR_ERROR
		%endif
	%endif		

	%ifndef STD_PAR_ERROR
		%if %0 = 2
			move_if_unequal rdi, %2
		%elif %0 = 3
			move_if_unequal rdi, %2
			move_if_unequal rsi, %3
		%elif %0 = 4
			move_if_unequal rdi, %2
			move_if_unequal rsi, %3
			move_if_unequal rdx, %4
		%elif %0 = 5
			move_if_unequal rdi, %2
			move_if_unequal rsi, %3
			move_if_unequal rdx, %4
			move_if_unequal rcx, %5
		%elif %0 = 6
			move_if_unequal rdi, %2
			move_if_unequal rsi, %3
			move_if_unequal rdx, %4
			move_if_unequal rcx, %5
			move_if_unequal r8, %6
		%elif %0 = 7
			move_if_unequal rdi, %2
			move_if_unequal rsi, %3
			move_if_unequal rdx, %4
			move_if_unequal rcx, %5
			move_if_unequal r8, %6
			move_if_unequal r9, %7
		%elif %0 > 7
			move_if_unequal rdi, %2
			move_if_unequal rsi, %3
			move_if_unequal rdx, %4
			move_if_unequal rcx, %5
			move_if_unequal r8, %6
			move_if_unequal r9, %7

			%rep (%0-7)
				%rotate -1
				push %1
			%endrep
		%endif

		CALL_FUNC FUNC_NAME

		%if %0 > 7
			add rsp, (%0-7)*8
		%endif
	%else
		%undef STD_PAR_ERROR
		%assign SD_Par (%0-1)
		%ifdef %1_parameter_count_var
			%error The function %1 takes a minimum parameter count of %1_parameter_count instead of SD_Par parameters
		%else
			%error The function %1 takes %1_parameter_count parameters instead of SD_Par parameters
		%endif
		%undef SD_Par
	%endif

	%undef CALL_FUNC
	%undef FUNC_NAME
%endmacro


%endif
