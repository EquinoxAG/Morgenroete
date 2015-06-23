%ifndef _MORGENROETE_CALLING_CONVENTIONS_INC_
%define _MORGENROETE_CALLING_CONVENTIONS_INC_

%include "mgr_debug.inc"
%include "mgr_string.inc"
%include "mgr_inst.inc"

%macro StdExtCall 1
	force_call %1
%endmacro

;1 = function name, 2 = parameter count, 3 = from which file, 4 = calling convention
%macro DefineCall 3-4 STDCALL_VX64GCC
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

%macro DefineVariableCall 3-4 STDCALL_VX64GCC
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
%macro DefinePolymorphicCall 4-5 STDCALL_VX64GCC
	%xdefine %1_parameter_count %2
	%xdefine %1_calling_convention %5
	%xdefine %1_call %4
%endmacro


%macro secure_call 1-2+
	%ifndef %1_parameter_count 
		%warning The function was not defined by DefineCall therefore secure_call can not ensure a secure call
	%else
		%if %0 = 1
			%1_calling_convention %1
		%else
			%1_calling_convention %1, %2
		%endif
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

%macro STDCALL_VX64GCC 1-*
	%xdefine CALL_FUNC %1_call	
	%xdefine FUNC_NAME %1

	%ifdef %1_parameter_count_var
		%if %1_parameter_count > (%0-2)
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
