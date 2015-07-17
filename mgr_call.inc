%ifndef _MORGENROETE_CALLING_CONVENTIONS_INC_
%define _MORGENROETE_CALLING_CONVENTIONS_INC_

%include "mgr_debug.inc"
%include "mgr_string.inc"
%include "mgr_inst.inc"
%include "mgr_stack.inc"

%macro StdExtCall 2
	force_call %1
%endmacro

%define STDCALL_GCC64 'STDCALL_VX64GCC'

%xdefine MGR_CALL_MODULE_SuspectList
%xdefine MGR_CALL_MODULE_NumSuspects 0
%xdefine MGR_FALSE 1
%xdefine MGR_TRUE 0



%macro ImportAllMgrFunctions 0
	%if MGR_CALL_MODULE_NumSuspects > 0
		%macro CheckDefinedShit 1-*
			%rep %0
				%ifnstr %1
					%ifndef %1_declared
						extern %1
					%endif
				%else
					%warning %1
				%endif
				%rotate 1
			%endrep
		%endmacro

		CheckDefinedShit MGR_CALL_MODULE_SuspectList
		%unmacro CheckDefinedShit 1-*
	%endif
%endmacro


; 1 = function name, 2 = parameter count
%macro ResoluteFunctionName 2
	%defstr RFN_SuspectStr %1
	SearchAndReplace RFN_FullFuncName, RFN_SuspectStr, '::', '@', RFN_NumReplaces
	%undef RFN_SuspectStr
	%undef RFN_NumReplaces

	%defstr ParStr %2
	HasPartStr RFN_IsVariadic,ParStr, '+'
	%undef ParStr


	%macro IsDefinedErr 1
		%ifdef %1_pc
			%xdefine MGR_RFuncName %1
		%else
			%fatal Unknown function %1
		%endif
	%endmacro

	%ifdef RFN_IsVariadic
		%undef RFN_IsVariadic
		%deftok RFN_FullFuncName RFN_FullFuncName
		IsDefinedErr RFN_FullFuncName
	%else
		%deftok RFN_FullFuncName RFN_FullFuncName
		%xdefine RFN_FullFuncName RFN_FullFuncName %+ _parcount_%2
		IsDefinedErr RFN_FullFuncName
	%endif

	%undef RFN_FullFuncName
	%unmacro IsDefinedErr 1
%endmacro


;1 = function name, 2-* = parameter names,
%macro DeclareFunction 1+
	%defstr NDF_SuspectStrTmp %1
	SearchAndReplace NDF_SuspectStr, NDF_SuspectStrTmp, '::','@',NDF_NumReplaces
	%undef NDF_SuspectStrTmp


	PartStringsByChar NDF_SuspectStr, '(', NDF_FuncName, NDF_ArgList
	%xdefine NDF_SuspectStr NDF_FuncName

	PartStringsByCharLast NDF_ArgList, ')', NDF_RealArgList, NDF_Unused
	%undef NDF_Unused
	%undef NDF_ArgList

	%ifdef NDF_RealArgList
		CutAllWhiteSpace NDF_ArgList, NDF_RealArgList
		%undef NDF_RealArgList

		%strlen NDF_NumArgs NDF_ArgList
	%else
		%xdefine NDF_NumArgs 0
	%endif

	%if NDF_NumArgs = 0
		%undef NDF_ArgList
	%else
		%deftok NDF_ArgTmpList NDF_ArgList
		SearchAndReplace NDF_NewShit, NDF_ArgList, ',',',', NDF_NumArgumentsIn
		%assign NDF_NumArgumentsIn (NDF_NumArgumentsIn+1)
		%undef NDF_NewShit
		%undef NDF_ArgTmpList
		%undef NDF_NumArgs
		%deftok NDF_ArgList NDF_ArgList
	%endif

	%undef NDF_FuncName



	%strcat NDF_FuncExists NDF_SuspectStr, '_pc'
	%deftok NDF_FuncExistsTmp NDF_FuncExists
	%defstr NDF_FuncExistsTmp NDF_FuncExistsTmp

	MGRStrcmp NDF_FuncExists, NDF_FuncExistsTmp, NDF_DeclareNonVariadic

	%undef NDF_FuncExists
	%undef NDF_FuncExistsTmp


		;1 = func name, 2 = arg number, 3 = arg list
		%macro DefineNonVariadicCall 2-3+
			%ifndef %1_pc
				%xdefine %1_call StdExtCall
				%xdefine %1_pc %2
				%xdefine %1_add_par 0
				%deftok %1_calling_convention STDCALL_GCC64
				%1:
					CreateStack %1_STACK
					%if %0 = 3
						DefineArguments %3
						%xdefine LAST_USED_ARGLIST %3
					%endif
					%xdefine LAST_DECLARED_FUNCTION %1
			%elifn %1_pc = %2
				%fatal The variadic call %1 must be declared with naming only the first %1_pc parameters of the function
			%else
				global %1
				%1:
					CreateStack %1_STACK
					%xdefine %1_declared
					%if %0 = 3
						DefineArguments %3
						%xdefine LAST_USED_ARGLIST %3
					%endif
					%xdefine LAST_DECLARED_FUNCTION %1
			%endif
		%endmacro


	%deftok NDF_FuncName NDF_SuspectStr

	%ifdef NDF_ArgList
		BringToString NDF_ArgListStr, NDF_ParameterCountU, NDF_ArgList
		%assign NDF_NumArgs (NDF_NumArgumentsIn)
	%endif

	%ifdef NDF_DeclareNonVariadic
		%undef NDF_DeclareNonVariadic
		%if NDF_NumReplaces > 0
			%xdefine NDF_ArgListStr 'this', %+ NDF_ArgListStr
			%assign NDF_NumArgs (NDF_NumArgumentsIn+1)
		%endif
		%xdefine NDF_FuncName NDF_FuncName %+ _parcount_ %+ NDF_NumArgs
	%else	
		%if NDF_NumReplaces > 0
			%xdefine NDF_ArgListStr 'this', %+ NDF_ArgListStr
			%assign NDF_NumArgs (NDF_NumArgumentsIn+1)
		%endif
	%endif

	%ifdef NDF_ArgList
		DefineNonVariadicCall NDF_FuncName, NDF_NumArgs,NDF_ArgListStr
	%else
		DefineNonVariadicCall NDF_FuncName, NDF_NumArgs
	%endif

	%undef NDF_NumArgs
	%undef NDF_ArgListStr
	%undef NDF_ParameterCountU
	%undef NDF_FuncName
	%undef NDF_NumArgumentsIn
	%unmacro DefineNonVariadicCall 2-3+
	%undef NDF_ArgList
	%undef NDF_SuspectStr
	%undef NDF_NumReplaces
%endmacro


;1 = function name, 2 = num parameters, 3 = which parameter, 4 = New macro to call the function
%macro ChangeFunctionAdditionalParameters 3-4 StdExtCall
	%defstr CFAP_UnderlyingStr %1

	SearchAndReplace CFAP_RealClassStr, CFAP_UnderlyingStr, '@', '::', Num_Replaces
	%undef Num_Replaces
	%deftok CFAP_RealClassStr CFAP_RealClassStr

	SC_IsDefined CFAP_IsFunc, CFAP_RealClassStr, _parcount_%2_pc

	%macro CFAP_DeepDefine 3
		%xdefine %1%2 %3
	%endmacro

	%ifdef CFAP_IsFunc
		%undef CFAP_IsFunc
		CFAP_DeepDefine CFAP_RealClassStr, _parcount_%2_add_par, %3
		CFAP_DeepDefine CFAP_RealClassStr, _parcount_%2_call, %4
	%else
		SC_IsDefined CFAP_IsFunc, CFAP_RealClassStr, _pc
		%ifdef CFAP_IsFunc
			%undef CFAP_IsFunc
			CFAP_DeepDefine CFAP_RealClassStr, _add_par, %3
			CFAP_DeepDefine CFAP_RealClassStr, _call, %4
		%else
			%fatal There is no function %1 with a parameter count of %2
		%endif
	%endif

	%undef CFAP_UnderlyingStr
	%undef CFAP_RealClassStr
	%unmacro CFAP_DeppDefine 3
%endmacro


;1 = function name, 2 = parameter count
%macro DefineFunction 2
	NDefineFunction_int %1, %2, STDCALL_GCC64, StdExtCall, 0
%endmacro

; 1 = function name , 2 = parameter count , 3 = calling convention
%macro DefineFunction 3
	NDefineFunction_int %1, %2, %3, StdExtCall, 0
%endmacro

%macro DefineFunction 4
	NDefineFunction_int %1, %2, %3, %4, 0
%endmacro

%macro DefineFunction 5
	NDefineFunction_int %1, %2, %3, %4, %5
%endmacro

;1 = function name, 2 = parameter count, 3 = calling convention
%macro NDefineFunction_int 5
		;1 = function name, 2 = parameter count, 3 = calling convention
		%macro CheckIfDefinedAFuncLikeThat 5
			%defstr CIDAFLK_TryStr %2
		
			
			%defstr CIDAFLK_FuncStr %1
			SearchAndReplace CIDA_Error_str, CIDAFLK_FuncStr, '@','::', Num_Replaces
			%undef Num_Replaces


			HasPartStr IsVariadic, CIDAFLK_TryStr, '+'

			%ifdef IsVariadic
				%undef IsVariadic
				%ifdef %1_func_exists
					%fatal You must not overload the function CIDA_Error_str with a variadic function!
				%else
					%xdefine %1_func_exists 0
				%endif

				CutAllCharactersOfType CIA_ParCntStr,CIDAFLK_TryStr,'+'
				%deftok CIA_ParCnt CIA_ParCntStr

				%deftok %1_calling_convention %3
				%xdefine %1_pc CIA_ParCnt
				%xdefine %1_call %4
				%xdefine %1_add_par %5


				%ifn MGR_CALL_MODULE_NumSuspects = 0
					%xdefine MGR_CALL_MODULE_SuspectList MGR_CALL_MODULE_SuspectList %+ ,
					%xdefine MGR_CALL_MODULE_SuspectList MGR_CALL_MODULE_SuspectList %+ %1
				%else
					%xdefine MGR_CALL_MODULE_SuspectList %1
				%endif
				%xdefine MGR_CALL_MODULE_NumSuspects (MGR_CALL_MODULE_NumSuspects+1)
				%undef CIA_ParCnt
				%undef CIA_ParCntStr
			%else
				%ifndef %1_func_exists
					%xdefine %1_func_exists 0
				%elifdef %1_pc
					%fatal You must not overload a variadic call with the function CIDA_Error_str!
				%endif

				%ifdef %1_parcount_%2_pc
					%fatal There is already a function CIDA_Error_str which takes %2 parameters
				%else
					%deftok %1_parcount_%2_calling_convention %3
					%xdefine %1_parcount_%2_pc %2
					%xdefine %1_parcount_%2_call %4
					%xdefine %1_parcount_%2_add_par %5

					%ifn MGR_CALL_MODULE_NumSuspects = 0
						%xdefine MGR_CALL_MODULE_SuspectList MGR_CALL_MODULE_SuspectList %+ ,
						%xdefine MGR_CALL_MODULE_SuspectList MGR_CALL_MODULE_SuspectList %+ %1_parcount_%2
					%else
						%xdefine MGR_CALL_MODULE_SuspectList %1_parcount_%2
					%endif

					%xdefine MGR_CALL_MODULE_NumSuspects (MGR_CALL_MODULE_NumSuspects+1)
				%endif
			%endif

			%undef CIDA_Error_str
			%undef CIDAFLK_TryStr
			%undef CIDAFLK_FuncStr
		%endmacro



	%defstr NDF_SuspectStr %1
	HasPartStr NDF_IsClassFunc,NDF_SuspectStr,'::'
	%ifdef NDF_IsClassFunc
		%undef NDF_IsClassFunc
		PartStringsByChar NDF_SuspectStr, ':', NDF_InterfaceName, NDF_TmpFuncName
		CutAllCharactersOfType NDF_FunctionName, NDF_TmpFuncName, ':'
		%undef NDF_TmpFuncName
		IsInterface NDF_InterfaceName, NDF_IsRealInterface

		%ifdef NDF_IsRealInterface
			%undef NDF_IsRealInterface
			%deftok NDF_InterfaceNameToken NDF_InterfaceName
			%deftok NDF_FunctionNameToken NDF_FunctionName
			%defstr NDF_ChangeParCount %2

			HasPartStr IsVariadic, NDF_ChangeParCount, '+'

			%ifdef IsVariadic
				%undef IsVariadic
				CutAllCharactersOfType NDF_NewParCount, NDF_ChangeParCount, '+'
				%deftok NDF_CurrParCount NDF_NewParCount
				%assign NDF_CurrParCount (NDF_CurrParCount+1)
				%xdefine NDF_RealParCount NDF_CurrParCount %+ +
				%undef NDF_NewParCount
				%undef NDF_CurrParCount
			%else
				%assign NDF_RealParCount (%2+1)
			%endif
			%undef NDF_ChangeParCount
	
		%else
			%fatal The interface NDF_InterfaceName does not exitst, therefore there cannot be a class function of this class
		%endif


		%xdefine NDF_ComplName NDF_InterfaceNameToken %+ @ %+ NDF_FunctionNameToken
		CheckIfDefinedAFuncLikeThat NDF_ComplName, NDF_RealParCount, %3, %4, %5
		%undef NDF_RealParCount
		%undef NDF_ComplName
		%undef NDF_InterfaceNameToken
		%undef NDF_FunctionNameToken
		%undef NDF_InterfaceName
		%undef NDF_FunctionName
	%else
		CheckIfDefinedAFuncLikeThat %1, %2, %3, %4, %5
	%endif

	%undef NDF_SuspectStr
	%unmacro CheckIfDefinedAFuncLikeThat 5
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

%macro SC_IsDefined 3-4
	%if %0 = 3
		%ifdef %2%3
			%xdefine %1 %2%3
		%endif
	%else
		%ifdef %2_parcount_%3%4
			%xdefine %1 %2_parcount_%3%4
		%endif
	%endif
%endmacro




%macro secure_call 1+
	%defstr SC_PickingStr %1

	HasPartStr SC_HasArgList,SC_PickingStr, '('
	%ifdef SC_HasArgList
		%undef SC_HasArgList

		HasPartStr SC_HasSynErr, SC_PickingStr, '.('

		%ifdef SC_HasSynErr
			%fatal Syntax error expression %1 makes no sense furthermore .( is not a valid syntax
		%endif

		HasPartStr SC_HasSynErr, SC_PickingStr, '::'
		%ifdef SC_HasSynErr
			%fatal Syntax error expression %1 makes no sense
		%endif

		%xdefine SC_TypDeductStr ''
		%xdefine SC_SurgeryStr SC_PickingStr
		%undef SC_PickingStr
		%rep 200
			%undef SC_STRPart0
			%undef SC_STRPart1
			PartStringsByChar SC_SurgeryStr, '(', SC_STRPart0, SC_STRPart1
			%strlen SC_TmpStrlen SC_STRPart0
			%if SC_TmpStrlen = 0
				%strcat SC_TypDeductStr SC_TypDeductStr, '('
				%xdefine SC_SurgeryStr SC_STRPart1
			%else
				PartStringsByCharLast SC_STRPart1,')', SC_ArgListImp, SC_ArgListTmp
				%undef SC_ArgListTmp
				%ifndef SC_ArgListImp
					%xdefine SC_ArgListImp ''
				%endif
				%exitrep
			%endif
		%endrep
		%strcat SC_TypDeductStr SC_TypDeductStr, SC_STRPart0
		%undef SC_TmpStrlen
		%undef SC_STRPart0
		%undef SC_STRPart1
		%undef SC_SurgeryStr

		HasPartStr SC_IsClassFunc, SC_TypDeductStr, '.'

		%ifdef SC_IsClassFunc
			%undef SC_IsClassFunc
			PartStringsByCharLast SC_TypDeductStr, '.', SC_InterfaceName, SC_FunctionName
			%deftok TmpIntName SC_InterfaceName
			SearchReplaceLocalVar TmpIntName, SC_DeleteInstant, SC_DeductedType
			%undef SC_DeleteInstant
			%deftok TmpIntName SC_FunctionName
			%xdefine SC_FunctionName SC_DeductedType %+ @ %+ TmpIntName
			%xdefine SC_IsClassFunc @ %+ TmpIntName
			%undef TmpIntName
			%strcat SC_ArgListImp ',', SC_ArgListImp
			%strcat SC_ArgListImp SC_InterfaceName, SC_ArgListImp
		%else
			%deftok SC_FunctionName SC_TypDeductStr
		%endif

		CutAllCharactersOfType SC_RealArgList, SC_ArgListImp, ' '



		%strlen SC_RealArgListLen SC_RealArgList

		%if SC_RealArgListLen = 0
			%define MAKECALL(x,y) CALL_CONV x
		%else
			%define MAKECALL(x,y) CALL_CONV x,y
		%endif


		%deftok SC_ArgListImp SC_RealArgList
		SearchAndReplace SC_NewArgList, SC_RealArgList, ',',',', SC_NumReplaces
		%undef SC_NewArgList

		%ifn SC_RealArgListLen = 0
			%assign SC_NumParameters (SC_NumReplaces+1)
		%else
			%assign SC_NumParameters 0
		%endif

		%undef SC_NumReplaces



		%ifdef SC_IsClassFunc
			GetInterfaceNumBaseClasses SC_DeductedType, SC_DepthSearches
		%else
			%xdefine SC_DepthSearches 1
		%endif

		%assign SC_LoopCounter 1

		%xdefine SC_HasCalled MGR_FALSE

		%rep SC_DepthSearches
		
			%ifdef SC_IsClassFunc
				%undef SC_CurrentDerivedClass
				GetBaseClassByNum SC_DeductedType, SC_LoopCounter, SC_CurrentDerivedClass
				%xdefine SC_FunctionName SC_CurrentDerivedClass %+ SC_IsClassFunc
			%endif


			%undef CALL_CONV
			SC_IsDefined CALL_CONV, SC_FunctionName, SC_NumParameters, _calling_convention

			%ifdef CALL_CONV
				%xdefine SC_FunctionName SC_FunctionName %+ _parcount_ %+ SC_NumParameters
				MAKECALL(SC_FunctionName,SC_ArgListImp)
				%xdefine SC_HasCalled MGR_TRUE
			%else
				SC_IsDefined CALL_CONV, SC_FunctionName, _calling_convention
				%ifdef CALL_CONV
					SC_IsDefined SC_RealParCnt, SC_FunctionName, _pc
					%ifn SC_RealParCnt > SC_NumParameters
						MAKECALL(SC_FunctionName, SC_ArgListImp)
						%xdefine SC_HasCalled MGR_TRUE
					%endif
					%undef SC_RealParCnt
				%endif
			%endif

			%assign SC_LoopCounter (SC_LoopCounter+1)
			%undef CALL_CONV
		%endrep

		%undef SC_LoopCounter
		%if SC_HasCalled = MGR_FALSE
			%fatal There is no function SC_FunctionName which takes SC_NumParameters parameters!
		%endif

		%undef SC_DepthSearches
		%undef SC_HasCalled
		%undef SC_IsClassFunc
		%undef MAKECALL
		%undef SC_NumParameters

		%undef SC_IsClassFunc
		%undef SC_RealArgListLen
		%undef SC_RealArgList
		%undef SC_ArgListImp
		%undef SC_DeductedType

	%else
		%fatal The function misses any argument list %1!
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
	%xdefine FUNC_ADD_PAR %1_add_par


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
	CALL_FUNC FUNC_NAME, FUNC_ADD_PAR
	%if %0 > 5
		add rsp, (32 + ((%0-5)*8))
	%else
		add rsp, 32
	%endif

	%undef CALL_FUNC
	%undef FUNC_NAME
	%undef FUNC_ADD_PAR
%endmacro

%macro STDCALL_VX64GCC 1-*
	%xdefine CALL_FUNC %1_call
	%xdefine FUNC_NAME %1
	%xdefine CALL_FUNC_ADD_PAR %1_add_par

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

		CALL_FUNC FUNC_NAME, CALL_FUNC_ADD_PAR

		%if %0 > 7
			add rsp, (%0-7)*8
		%endif

	%undef CALL_FUNC
	%undef FUNC_NAME
	%undef CALL_FUNC_ADD_PAR
%endmacro


%endif
