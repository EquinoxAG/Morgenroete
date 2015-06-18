%ifndef _MORGENROETE_MASTER_FILE_
%define _MORGENROETE_MASTER_FILE_

%xdefine NUM_STACKS_OPEN 0

;----------------------------- CALLING Conventions --------------------------


%macro move_if_unequal 2
	%defstr MIU_Tok0 %1
	%defstr MIU_Tok1 %2

	%ifn MIU_Tok0 = MIU_Tok1
		mov %1, %2
	%endif
	
	%undef MIU_Tok0
	%undef MIU_Tok1

%endmacro

%macro STDCALL_VX64GCC 1-*
	%xdefine CALL_FUNC %1	

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
	%else
		move_if_unequal rdi, %2
		move_if_unequal rsi, %3
		move_if_unequal rdx, %4
		move_if_unequal rcx, %5
		move_if_unequal r8, %6
		move_if_unequal r9, %7

		%rep (%0-7)
			%rotate -1
			%push %1
		%endrep
	%endif

	call CALL_FUNC

	%if %0 > 7
		add rsp, (%0-7)*8
	%endif
%endmacro

%define STD_CALLING_CONVENTION STDCALL_VX64GCC

%macro DefineSTDCALLGCC 1
	%define %1_CALLING_CONVENTION STDCALL_VX64GCC
%endmacro

;---------------------------------------------------------------------------

;1 = interface name, 2 = type_name, 3 = type offset, 4 = type length
%macro interface 4
	%ifdef %1_size
		%if %1_size < (%3+%4)
			%xdefine %1_size (%3+%4)
			%xdefine %1.%2 %3
			%xdefine %1_%2_LENGTH %4
		%else
			%xdefine %1.%2 %3
			%xdefine %1_%2_LENGTH %4
		%endif
		%assign %1_parameter (%1_parameter+1)
		RegisterByNum %1, %1_parameter, %2
	%else
		%xdefine %1_size (%3+%4)
		%xdefine %1.%2 %3
		%xdefine %1_%2_LENGTH %4
		%assign %1_parameter 1
		%xdefine %1_constructor_pc 0
		RegisterByNum %1, %1_parameter, %2
	%endif
%endmacro

%macro interface_constructor 3
	%ifndef %1_size
		%error No interface with the name %1
	%else
		%ifdef %1_constructor
			%warning Overwriting the constructor of the interface %1
		%endif
		%xdefine %1_constructor %2
		%xdefine %1_constructor_pc %3
	%endif
%endmacro

%macro interface_destructor 2
	%ifndef %1_size
		%error No interface with the name %1
	%else
		%ifdef %1_destructor
			%warning Overwriting the destructor of the interface %1
		%endif
			%xdefine %1_destructor %2
	%endif
%endmacro

;1 = name, 2 = num, 3 = membername
%macro RegisterByNum 3
	%xdefine %1_%2 %3
%endmacro

;name, num
%macro UnregisterByNum 2
	InDepthUnregister %1, %1_%2 
	%undef %1_%2
%endmacro

%macro InDepthUnregister 2
	%undef %1.%2
	%undef %1_%2_LENGTH
%endmacro

;1 = name
%macro uninterface 1
	%ifndef %1_parameter
		%error Unknown Interface
	%else
		%undef %1_size
		%undef %1_constructor
		%undef %1_constructor_pc
		%undef %1_destructor

		%assign counter 1
		%rep %1_parameter
			UnregisterByNum %1, counter
			%assign counter (counter+1)
		%endrep
		%undef counter
		
		%undef %1_parameter
	%endif
%endmacro


%macro interface 3
	%ifdef %1_size
		interface %1, %2, %1_size, %3
	%else
		interface %1, %2, 0, %3
	%endif
%endmacro

%macro DefineByNum 3
	InDepthDefine %1, %2, %2_%3
%endmacro

%macro UndefByNum 3
	InDepthUnregister %1, %2_%3
%endmacro

%macro InDepthDefine 3
	%ifdef %1.%3
		%error Variale %1 . %2 is already defined
	%else
		%define %1.%3 (%1 + %2.%3)
	%endif
%endmacro


; 1 = Heap name, 2 = interface name
%macro ConnectHeapToInterface 2
	%ifndef %2_parameter
		%error Unknown interface
	%else
		%ifdef %1_MRGROE_CONNECTED_INTERFACE
			UnconnectHeapFromInterface %1
		%endif

		%xdefine %1_MRGROE_CONNECTED_INTERFACE %2
		%assign counter 1

		%rep %2_parameter
			DefineByNum %1, %2, counter
			%assign counter (counter+1)
		%endrep
		%undef counter
	%endif
%endmacro



%macro InternUnconnectHeapFromInterface 2
	%ifndef %1_MRGROE_CONNECTED_INTERFACE
		%error Unknown interface
	%else
		%assign counter 1
	
		%rep %2_parameter
			UndefByNum %1, %2, counter
			%assign counter (counter+1)
		%endrep
		%undef counter
	%endif
%endmacro

%macro UnconnectHeapFromInterface 1
	%ifndef %1_MRGROE_CONNECTED_INTERFACE
		%error Unknown interface
	%else
		InternUnconnectHeapFromInterface %1, %1_MRGROE_CONNECTED_INTERFACE	
		%undef %1_MRGROE_CONNECTED_INTERFACE
	%endif
%endmacro


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
		%xdefine NUM_STACKS_OPEN (NUM_STACKS_OPEN+1)

	
		ReserveStackSpace SaveRbxPtr, qword
		ReserveStackSpace SaveR10Ptr, qword
		ReserveStackSpace SaveR11Ptr, qword
		ReserveStackSpace SaveR12Ptr, qword
	%endif
%endmacro

%macro ReserveStackSpace 2-*
	%if %0 < 3
		InternReserveStackSpace %1, %2, %$STACK_NAME
	%else
		InternReserveStackSpace %1, %2, %$STACK_NAME, %{3:-1}
	%endif
%endmacro

%macro InternReserveStackSpace 3-*
	%ifndef STACK_NAME_%3
		%error No current Stack available
	%else
		%defstr RSVSP_trystr %$%1
		%strcat RSVSP_trystr RSVSP_trystr, '_VAR'
		IsLocalDefined VAR_EXISTS, RSVSP_trystr
		%undef RSVSP_trystr

		%ifndef VAR_EXISTS
			%defstr TryStr %2
			%if TryStr = 'byte'
				%xdefine STACK_SPACE_%3_FLOATING (STACK_SPACE_%3_FLOATING + 1)
			%elif TryStr = 'word'
				%xdefine STACK_SPACE_%3_FLOATING (STACK_SPACE_%3_FLOATING + 2)
			%elif TryStr = 'dword'
				%xdefine STACK_SPACE_%3_FLOATING (STACK_SPACE_%3_FLOATING + 4)
			%elif TryStr = 'qword'
				%xdefine STACK_SPACE_%3_FLOATING (STACK_SPACE_%3_FLOATING + 8)
			%else
				%xdefine STACK_SPACE_%3_FLOATING (STACK_SPACE_%3_FLOATING + %2_size)
			%endif
	
			%xdefine %$%1_VAR STACK_SPACE_%3_FLOATING
			%xdefine %$%1_STRUC %2

			%ifdef %2_constructor
				%ifn (%0-3) = %2_constructor_pc
					%assign ArgNum (%0-3)
					%error The constructor of the struct %2 takes %2_constructor_pc arguments, not ArgNum
					%undef ArgNum
				%else
					%ifn %2_constructor_pc = 0
						DefineNewStr %$%1_CONSTRUCTOR_LIST, %{4:-1}
					%endif
				%endif

			

				%xdefine %$%1_CONSTRUCTOR %2_constructor

				%defstr IRSS_CstStr %1
				%strlen IRSS_CstStrlen %$CONSTRUCTOR_LIST
				%ifn IRSS_CstStrlen = 0
					%strcat %$CONSTRUCTOR_LIST %$CONSTRUCTOR_LIST,','
				%endif
				%undef IRSS_CstStrlen

				%strcat %$CONSTRUCTOR_LIST %$CONSTRUCTOR_LIST, '%$'
				%strcat %$CONSTRUCTOR_LIST %$CONSTRUCTOR_LIST, IRSS_CstStr

				%undef IRSS_CstStr
			%endif

			%ifdef %2_destructor
				%strlen IRSS_DstStrlen %$DESTRUCTOR_LIST
				%ifn IRSS_DstStrlen=0
					%strcat %$DESTRUCTOR_LIST %$DESTRUCTOR_LIST,','
				%endif
				%undef IRSS_DstStrlen

				%defstr IRSS_DstStr %1
				%xdefine %$%1_DESTRUCTOR %2_destructor

				%strcat %$DESTRUCTOR_LIST %$DESTRUCTOR_LIST, '%$'
				%strcat %$DESTRUCTOR_LIST %$DESTRUCTOR_LIST, IRSS_DstStr
				%undef IRSS_DstStr				
			%endif		

			%undef TryStr
		%else
			%error A variable with the name %1 already exists
			%undef VAR_EXISTS
		%endif
	%endif
%endmacro

;1 = new StringName
%macro DefineNewStr 2-*
	%defstr DNS_NewStr %2
	%rep (%0-2)
		%rotate -1
		%strcat DNS_NewStr DNS_NewStr,','
		%defstr DNS_appendStr %1
		%strcat DNS_NewStr DNS_NewStr, DNS_appendStr
	%endrep
	%undef DNS_appendStr
	%rotate -1
	%rotate -1
	%deftok %1 DNS_NewStr
	%undef DNS_NewStr
%endmacro

%macro MGR_call_destructors 0
	%strlen MGRCD_strlen %$DESTRUCTOR_LIST
	%ifn MGRCD_strlen = 0
		mov_stack qword[ SaveRbxPtr ], rbx
		%deftok MGRCD_ArgList %$DESTRUCTOR_LIST
		call_dst MGRCD_ArgList
		%undef MGRCD_ArgList
		%xdefine %$DESTRUCTOR_LIST ''
		mov_stack rbx, qword[ SaveRbxPtr ]
	%endif
	%undef MGRCD_strlen
%endmacro

%macro call_dst 1-*
	%rep %0
		%defstr CurrStr %1
		%defstr CurrDst %1
		%strcat CurrStr CurrStr,'_VAR'
		%strcat CurrDst CurrDst,'_DESTRUCTOR'
		%deftok CurrStrTok CurrStr
		%deftok CurrDstTok CurrDst
		InDepthCallDst CurrStrTok, CurrDstTok
		%undef CurrStrTok
		%undef CurrDstTok
		%undef CurrDst
		%undef CurrStr
		%rotate 1
	%endrep
%endmacro

%macro InDepthCallDst 2
	mov rbx, rbp
	sub rbx, %1
	
	%ifdef %2_CALLING_CONVENTION
		%2_CALLING_CONVENTION %2,rbx
	%else
		STD_CALLING_CONVENTION %2,rbx
	%endif
%endmacro

%macro MGR_call_constructors 0
	%strlen MGRCC_strlen %$CONSTRUCTOR_LIST
	%ifn MGRCC_strlen = 0
		mov_stack qword[ SaveRbxPtr ], rbx
		%deftok ArgList %$CONSTRUCTOR_LIST
		call_cnst ArgList
		%undef ArgList
		%xdefine %$CONSTRUCTOR_LIST ''
		mov_stack rbx,qword[ SaveRbxPtr ]
	%endif
	%undef MGRCC_strlen
%endmacro

%macro call_cnst 1-*
	%rep %0
		%defstr CurrStr %1
		%defstr CurrVar %1
		%defstr CurrCstList %1
		%defstr CurrStruc %1
		%strcat CurrStr CurrStr, '_CONSTRUCTOR'
		%strcat CurrVar CurrVar, '_VAR'
		%strcat CurrStruc CurrStruc, '_STRUC'
		%strcat CurrCstList CurrCstList, '_CONSTRUCTOR_LIST'
		%deftok CurrTok CurrStr
		%deftok CurrVarTok CurrVar
		%deftok CurrCstTok CurrCstList
		%deftok CurrStrucTok CurrStruc
		InDepthCall CurrVarTok, CurrTok, CurrStrucTok,CurrCstTok
		%undef CurrStrucTok
		%undef CurrStruc
		%undef CurrCstList
		%undef CurrCstTok
		%undef CurrStr
		%undef CurrVarTok
		%undef CurrVar
		%undef CurrTok
		%rotate 1
	%endrep
%endmacro

%macro InDepthCall 4-*
	mov rbx, rbp
	sub rbx, %1
	
	%ifdef %2_CALLING_CONVENTION
		%if %3_constructor_pc > 0
			%2_CALLING_CONVENTION %2,rbx,%{4:-1}
		%else
			%2_CALLING_CONVENTION %2,rbx
		%endif
	%else
		%if %3_constructor_pc > 0
			STD_CALLING_CONVENTION %2,rbx,%{4:-1}
		%else
			STD_CALLING_CONVENTION %2,rbx
		%endif
	%endif
%endmacro

;1 = string, 2 = terminated count, 3 = strname, 4 = str2name
%macro DivideStringsByChar 4
	%xdefine DSBC_counter 1
	%strlen DSBC_strlen %1
	%xdefine DSBC_is_sth 0
	
	%rep DSBC_strlen
		%substr DSBC_current_char %1 DSBC_counter
		%if DSBC_current_char = %2
			%substr %3 %1 1, (DSBC_counter-1)
			%substr %4 %1 (DSBC_counter+1), DSBC_strlen
			%xdefine DSBC_is_sth 1
		%endif
		%xdefine DSBC_counter (DSBC_counter+1)
	%endrep
	
	%if DSBC_is_sth = 0
		%xdefine %3 %1
	%endif
	
	%undef DSBC_is_sth

	%undef DSBC_counter
	%undef DSBC_strlen
	%undef DSBC_current_char
%endmacro

%macro mov_stack 2
		FindAndReplaceLocalVar %1, OP0
		FindAndReplaceLocalVar %2, OP1
		%xdefine DISABLE_INST_MACRO_MOV
		mov OP0, OP1
		%undef DISABLE_INST_MACRO_MOV
		%undef OP0
		%undef OP1
%endmacro



%macro mov 2
	%ifdef DISABLE_INST_MACRO_MOV
		mov %1, %2
	%else
		%if NUM_STACKS_OPEN = 0
			mov %1, %2
		%else
			UpdateStackPtr
			FindAndReplaceLocalVar %1, OP0
			FindAndReplaceLocalVar %2, OP1
			mov OP0, OP1
			%undef OP0
			%undef OP1
		%endif
	%endif
%endmacro

;1 = string 1, 2 = string 2, 3 =defined if equal
%macro MGR_strcmp 3
	%strlen MGRSTRLEN_strlen0 %1
	%strlen MGRSTRLEN_strlen1 %2

	%if MGRSTRLEN_strlen0 = MGRSTRLEN_strlen1
		%define MGRSTRCMP_ISDEF

		%xdefine MGRSTRLEN_counter 1
		%rep MGRSTRLEN_strlen0
			%substr MGRSTRLEN_str0 %1 MGRSTRLEN_counter
			%substr MGRSTRLEN_str1 %2 MGRSTRLEN_counter
			%ifn MGRSTRLEN_str0 = MGRSTRLEN_str1
				%undef MGRSTRCMP_ISDEF
			%endif
			%xdefine MGRSTRLEN_counter (MGRSTRLEN_counter+1)	
		%endrep

		%ifdef MGRSTRCMP_ISDEF
			%define %3
			%undef MGRSTRCMP_ISDEF
		%endif

		%undef MGRSTRLEN_counter
		%undef MGRSTRLEN_str0
		%undef MGRSTRLEN_str1
	%endif
	%undef MGRSTRLEN_strlen0
	%undef MGRSTRLEN_strlen1
%endmacro

;1 = expression, 2 = Fixed Token name
%macro FindAndReplaceLocalVar 2
	%xdefine %2 %1
	%defstr UnderlyingStr %1
	%assign FARLV_counter 1
	%assign FARLV_counter_end 1
	%strlen FARLV_strlen UnderlyingStr

	%rep FARLV_strlen
		%substr FARLV_character UnderlyingStr FARLV_counter_end
		%if FARLV_character = '['
			%xdefine FARLV_SUSPECT
		%elif FARLV_character = ']'
			%xdefine FARLV_SUSPECT
		%elif FARLV_character = ' '
			%xdefine FARLV_SUSPECT
		%elif FARLV_counter_end = FARLV_strlen
			%xdefine FARLV_SUSPECT
			%assign FARLV_counter_end (FARLV_counter_end+1)
		%endif

		%ifdef FARLV_SUSPECT	
			%undef FARLV_SUSPECT
			%substr FARLV_varstr UnderlyingStr FARLV_counter, (FARLV_counter_end-FARLV_counter)
			
			%undef FARLV_STR0
			%undef FARLV_STR1
			DivideStringsByChar FARLV_varstr,'.',FARLV_STR0,FARLV_STR1 
		
			%strcat FARLV_STR0 '%$', FARLV_STR0
			%strcat FARLV_var FARLV_STR0, '_VAR'
		
			IsLocalDefined VAR_EXISTS, FARLV_var
	
			%ifdef VAR_EXISTS
				%undef VAR_EXISTS	
				%ifdef FARLV_STR1
					%strcat FARLV_struc FARLV_STR0, '_STRUC'
					%deftok FARLV_ending FARLV_STR1
					%deftok FARLV_beg FARLV_struc
					%xdefine FARLV_together FARLV_beg %+ . %+ FARLV_ending
					%defstr FARLV_offsetstr FARLV_together
					%strcat FARLV_var FARLV_var, '-'
					%strcat FARLV_var FARLV_var, FARLV_offsetstr
				
					%undef FARLV_ending
					%undef FARLV_STR1
					%undef FARLV_beg
					%undef FARLV_together
					%undef FARLV_offsetstr
				%endif			

				%deftok FARLV_realoffset FARLV_var
				%xdefine FARLV_complete rbp - (FARLV_realoffset)
				%defstr FARLV_completestr FARLV_complete
				%substr FARLV_ENDSTR UnderlyingStr 1,(FARLV_counter-1)
				%strcat FARLV_ENDSTR FARLV_ENDSTR, FARLV_completestr
				%substr FARLV_endStr UnderlyingStr (FARLV_counter_end+1),(FARLV_strlen-FARLV_counter_end)
				%strcat FARLV_ENDSTR FARLV_ENDSTR, FARLV_endStr
				%deftok %2 FARLV_ENDSTR
	
				%undef FARLV_endStr
				%undef FARLV_ENDSTR
				%undef FARLV_realoffset
				%undef FARLV_complete
				%undef FARLV_completestr		

			%endif	
			%assign FARLV_counter (FARLV_counter_end+1)
		%endif

		%assign FARLV_counter_end (FARLV_counter_end+1)
	%endrep
	

	%undef UnderlyingStr
	%undef FARLV_counter_end
	%undef FARLV_counter
%endmacro

%macro UpdateStackPtr 0
	InternUpdateStackPtr %$STACK_NAME
%endmacro

%macro InternUpdateStackPtr 1
	%ifn STACK_SPACE_%1_CURRENT = STACK_SPACE_%1_FLOATING
		sub rsp, (STACK_SPACE_%1_FLOATING - STACK_SPACE_%1_CURRENT)
		%xdefine STACK_SPACE_%1_CURRENT (STACK_SPACE_%1_FLOATING)
		MGR_call_constructors	
	%endif
%endmacro

;name of to define if defined, Local variable as str
%macro IsLocalDefined 2
	%deftok TryTokenILD %2
	InternIsLocalDefined %1, %2, TryTokenILD
	%undef TryTokenILD
%endmacro

%macro InternIsLocalDefined 3
	%defstr TryStrILD %3

	MGR_strcmp TryStrILD, %2, ILD_TRY_STR
	%ifdef ILD_TRY_STR
		%undef ILD_TRY_STR
	%else
		%xdefine %1 0
	%endif
	%undef TryStrILD
%endmacro


%macro DestroyStack 1
	%ifdef STACK_NAME_%1
		MGR_call_destructors
		%xdefine NUM_STACKS_OPEN (NUM_STACKS_OPEN-1)
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
