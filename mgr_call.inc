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
%macro DefineFunction 3-4 'STDCALL_VX64GCC'
	DefineFunctionInterface %1, %2, %3, StdExtCall, 0, %4
%endmacro

;1 = function name, 2 = parameter count
%macro DeclareFunction 1
	%ifdef LAST_DECLARED_FUNCTION
		%fatal Before declaring a new call, close the last call with EndFunction first!
	%else
		%ifdef %1_parameter_count
			%ifn %1_parameter_count = (%0-1)
				%fatal The parameter count differs in declaration and definition, in function %1
			%else
				global %1
				%1:
					MGRStrcmp %1_calling_convention, 'STDCALL_VX64GCC', DC_IsEqual
					%ifndef DC_IsEqual
					%endif
					%undef DC_IsEqual
					CreateStack %1_STACK
					%xdefine LAST_DECLARED_FUNCTION %1
			%endif
		%else
			%xdefine %1_parameter_count 0
			%xdefine %1_calling_convention 'STDCALL_VX64GCC'
			%xdefine %1_call StdExtCall

			%1:
				CreateStack %1_STACK
				%xdefine LAST_DECLARED_FUNCTION %1
		%endif
	%endif
%endmacro

;1 = function name, 2 = parameter count
%macro DeclareFunction 2+
	%ifdef LAST_DECLARED_FUNCTION
		%fatal Before declaring a new call, close the last call with EndFunction first!
	%else
		;1=to define if equal, 2 = num args needed
		%macro IsEqualInNums 2-*
			%if %2 = (%0-2)
				%xdefine %1
			%else
				%undef %1
			%endif
		%endmacro

		%macro IsGreaterThen 2-*
			%if %2 < (%0-2)		;2 + Grater Than
				%xdefine %1
			%else
				%undef %1
			%endif
		%endmacro


		%ifdef %1_parameter_count
			
			%ifdef %1_parameter_count_var
				IsGreaterThen DC_ISEqual, %1_parameter_count, %2
			%else
				IsEqualInNums DC_ISEqual, %1_parameter_count, %2
			%endif

			%ifndef DC_ISEqual
				%fatal The parameter count differs in declaration and definition, in function %1
			%else
				BringToString ARG_LIST_DC,DC_ARG_LIST_NUM, %2	
				%undef DC_ARG_LIST_NUM

				%undef DC_ISEqual
				global %1
				%1:
					MGRStrcmp %1_calling_convention, 'STDCALL_VX64GCC', DC_IsEqual
					%ifndef DC_IsEqual
					%endif
					%undef DC_IsEqual
					CreateStack %1_STACK
					DefineArguments ARG_LIST_DC
					%xdefine LAST_DECLARED_FUNCTION %1
					%xdefine LAST_USED_ARGLIST ARG_LIST_DC
				%undef ARG_LIST_DC
			%endif
		%else
			%xdefine %1_calling_convention 'STDCALL_VX64GCC'
			%xdefine %1_call StdExtCall

			BringToString ARG_LIST_DC, %1_parameter_count, %2
			%1:
				CreateStack %1_STACK
				%xdefine LAST_DECLARED_FUNCTION %1
				%xdefine LAST_USED_ARGLIST ARG_LIST_DC
				DefineArguments ARG_LIST_DC
			%undef ARG_LIST_DC

		%endif

		%unmacro IsEqualInNums 2-*
		%unmacro IsGreaterThen 2-*


	%endif
%endmacro

;1 = to define, 2 = number of parameters, 3 = parameters
%macro BringToString 3-*
	%undef BTS_EndStr

	%define %2 (%0-2)
	%rep (%0-2)
		%ifndef BTS_EndStr
			%ifstr %3
				%xdefine BTS_EndStr %3
			%else
				%defstr BTS_String %3
				%xdefine BTS_EndStr BTS_String
			%endif
		%else
			%ifstr %2
				%xdefine BTS_EndStr BTS_EndStr %+ ,
				%xdefine BTS_EndStr BTS_EndStr %+ %3
			%else
				%defstr BTS_String %3
				%xdefine BTS_EndStr BTS_EndStr %+ ,
				%xdefine BTS_EndStr BTS_EndStr %+ BTS_String
			%endif
		%endif
		%rotate 1
	%endrep

	%rotate 1
	%rotate 1
	%xdefine %1 BTS_EndStr

	%undef BTS_EndStr
	%undef BTS_String
%endmacro


;1 = name
%macro EndFunction 0
	%xdefine STACK_NAME LAST_DECLARED_FUNCTION %+ _STACK
	DestroyStack STACK_NAME
	%undef STACK_NAME
	%ifdef LAST_USED_ARGLIST
		UndefineArguments LAST_USED_ARGLIST
	%endif
	%undef LAST_DECLARED_FUNCTION
	%undef LAST_USED_ARGLIST
	ret
%endmacro

%macro DefineVariableFunction 3-4 'STDCALL_VX64GCC'
	%xdefine %1_parameter_count_var
	DefineFunctionInterface %1, %2, %3, StdExtCall, 0, %4
%endmacro


;1 = function name, 2 = parameter count,4 = macro to call the function takes 1 parameter, 5 = calling convention
%macro DefinePolymorphicFunction 5-6 'STDCALL_VX64GCC'
	DefineFunctionInterface %1, %2, %3, %4, %5, %6
%endmacro

%macro DefineFunctionInterface 6
	%xdefine %1_parameter_count %2
	%xdefine %1_calling_convention %6
	%xdefine %1_call %4
	%xdefine %1_additional_parameter %5

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
		extern %1
	%endif
	%undef STR_STRONG
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

%macro DefineArguments 1-*
		%macro InternDefineArguments 3
			%strcat IDA_CurrShit %1, %2
			%deftok IDA_CurrShitTok IDA_CurrShit
			%macro InternDefineSDA 2
				%xdefine %1 %2
			%endmacro
			InternDefineSDA IDA_CurrShitTok, %3
			%unmacro InternDefineSDA 2
			%undef IDA_CurrShit
			%undef IDA_CurrShitTok
		%endmacro
		%if %0 > 0
			InternDefineArguments 'Arg_',%1,rdi
			%if %0 > 1
				InternDefineArguments 'Arg_',%2,rsi
				%if %0 > 2
					InternDefineArguments 'Arg_',%3,rdx
					%if %0 > 3
						InternDefineArguments 'Arg_',%4,rcx
						%if %0 > 4
							InternDefineArguments 'Arg_',%5,r8
							%if %0 > 5
								InternDefineArguments 'Arg_',%6,r9
								%if %0 > 6
									%assign DA_Counter 8
									%rep (%0-6)
										%rotate -1
										InternDefineArguments 'Arg_', %1, rbp+(DA_Counter*8) 
										%assign DA_Counter (DA_Counter+1)
									%endrep
									%undef DA_Counter
								%endif
							%endif
						%endif
					%endif
				%endif
			%endif
		%endif

		%unmacro InternDefineArguments 3
%endmacro

%macro UndefineArguments 1-*
	%macro InternUndefineArguments 1
		%xdefine FRST_ARG Ar
		%strcat SEC_ARG 'g_', %1
		%deftok SEC_ARG_TOK SEC_ARG
		%macro InternRealUndefine 2
			%undef %1%2
		%endmacro
		InternRealUndefine FRST_ARG,SEC_ARG_TOK
		%unmacro InternRealUndefine 2
		%undef FRST_ARG
		%undef SEC_ARG
		%undef SEC_ARG_TOK
	%endmacro
	%rep %0
		InternUndefineArguments %1
		%rotate 1
	%endrep
	%unmacro InternUndefineArguments 1
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
			%fatal The function %1 takes a minimum parameter count of %1_parameter_count instead of SD_Par parameters
		%else
			%fatal The function %1 takes %1_parameter_count parameters instead of SD_Par parameters
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
			%fatal The function %1 takes a minimum parameter count of %1_parameter_count instead of SD_Par parameters
		%else
			%fatal The function %1 takes %1_parameter_count parameters instead of SD_Par parameters
		%endif
		%undef SD_Par
	%endif

	%undef CALL_FUNC
	%undef FUNC_NAME
%endmacro


%endif
