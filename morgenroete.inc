%ifndef _MORGENROETE_MASTER_FILE_
%define _MORGENROETE_MASTER_FILE_

%xdefine NUM_STACKS_OPEN 0

;----------------------------- CALLING Conventions --------------------------

%ifndef byte_size
	%xdefine byte_size 1
%endif

%ifndef word_size
	%xdefine word_size 2
%endif

%ifndef dword_size
	%xdefine dword_size 4
%endif

%ifndef qword_size
	%xdefine qword_size 8
%endif

; 1 = current reg str, 2 = to defined reg
%macro reg64_toreg32 2
	%if %1 = 'rax'
		%xdefine %2 eax
	%elif %1 = 'rbx'
		%xdefine %2 ebx
	%elif %1 = 'rcx'
		%xdefine %2 ecx
	%elif %1 = 'rdx'
		%xdefine %2 edx
	%elif %1 = 'rsi'
		%xdefine %2 esi
	%elif %1 = 'rdi'
		%xdefine %2 edi
	%elif %1 = 'r8'
		%xdefine %2 r8d
	%elif %1 = 'r9'
		%xdefine %2 r9d
	%elif %1 = 'r10'
		%xdefine %2 r10d
	%elif %1 = 'r11'
		%xdefine %2 r11d
	%elif %1 = 'r12'
		%xdefine %2 r12d
	%elif %1 = 'r13'
		%xdefine %2 r13d
	%elif %1 = 'r14'
		%xdefine %2 r14d
	%else
		%xdefine %2 r15d
	%endif
%endmacro

%macro move_if_unequal 2
	%defstr MIU_Tok0 %1
	%defstr MIU_Tok1 %2

	MGR_strcmp MIU_Tok0, MIU_Tok1, MIU_IS_EQUAL

	%ifndef MIU_IS_EQUAL
		DeductType MIU_TYPE_SIZE, MIU_Tok1
		%ifdef MIU_TYPE_SIZE
			%ifn MIU_TYPE_SIZE = 8
				%ifn MIU_TYPE_SIZE = 4
					FindAndReplaceLocalVar %2, MIU_OPERAND
					%if MIU_OPERAND_SIZE = 4
						reg64_toreg32 MIU_Tok0, MIU_NewOP			
						mov MIU_NewOP, %2
						%undef MIU_NewOP
					%else
						%if MIU_OPERAND_SIZE = 2
							movzx %1, word%2
						%else
							movzx %1, byte%2
						%endif
					%endif
					%undef MIU_OPERAND
					%undef MIU_OPERAND_SIZE
				%else
					reg64_toreg32 MIU_Tok0, MIU_NewOP			
					mov MIU_NewOP, %2
					%undef MIU_NewOP
				%endif
			%else
				mov %1, %2
			%endif
			%undef MIU_TYPE_SIZE
		%else
			mov %1, %2
		%endif
	%else
		%undef MIU_IS_EQUAL
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
			push %1
		%endrep
	%endif

	force_call CALL_FUNC

	%if %0 > 7
		add rsp, (%0-7)*8
	%endif
%endmacro

%define STD_CALLING_CONVENTION STDCALL_VX64GCC

;To define if defined, 2 = string
%macro DeductType 2	
	CutAllWhiteSpace DT_UnderlyingString, %2

	%deftok DT_IS_NUM DT_UnderlyingString
	%ifnum DT_IS_NUM
		%if DT_IS_NUM < 0x100
			%xdefine %1 1
		%elif DT_IS_NUM < 0x10000
			%xdefine %1 2
		%elif DT_IS_NUM < 0x100000000
			%xdefine %1 4
		%else
			%xdefine %1 8
		%endif
	%else
		MultipleStrCmp DT_IS64, DT_UnderlyingString, 'rax', 'rbx', 'rcx', 'rdx', 'rsi', 'rdi', 'rsp', 'rbp', 'r8', 'r9','r10','r11','r12','r13','r14','r15'
	
		%ifdef DT_IS64
			%undef DT_IS64
			%xdefine %1 8
		%endif
	

		MultipleStrCmp DT_IS32, DT_UnderlyingString, 'eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'esp', 'ebp', 'r8d', 'r9d','r10d','r11d','r12d','r13d','r14d','r15d'

		%ifdef DT_IS32
			%undef DT_IS32
			%xdefine %1 4
		%endif

		MultipleStrCmp DT_IS16, DT_UnderlyingString, 'ax', 'bx', 'cx', 'dx', 'si', 'di', 'sp', 'bp', 'r8w', 'r9w','r10w','r11w','r12w','r13w','r14w','r15w'

		%ifdef DT_IS16
			%undef DT_IS16
			%xdefine %1 2
		%endif

		MultipleStrCmp DT_IS8, DT_UnderlyingString, 'al', 'ah', 'bl', 'bh', 'cl', 'ch', 'dl', 'dh'
		%ifdef DT_IS8
			%undef DT_IS8
			%xdefine %1 1
		%endif

		HasPartStr DT_IS8, DT_UnderlyingString, 'byte['
		%ifdef DT_IS8
			%undef DT_IS8
			%xdefine %1 1
		%endif
	
		HasPartStr DT_IS16, DT_UnderlyingString, 'word['
		%ifdef DT_IS16
			%undef DT_IS16
			%xdefine %1 2
		%endif
	
		HasPartStr DT_IS32, DT_UnderlyingString, 'dword['
		%ifdef DT_IS32
			%undef DT_IS32
			%xdefine %1 4
		%endif

		HasPartStr DT_IS64, DT_UnderlyingString, 'qword['
		%ifdef DT_IS64
			%undef DT_IS64
			%xdefine %1 8
		%endif

		HasPartStr DT_ISVAR, DT_UnderlyingString,'['
		%ifdef DT_ISVAR
			%undef DT_ISVAR
			%ifndef %1
				%xdefine %1 177
			%endif
		%endif	
	%endif
	%undef DT_IS_NUM

	%undef DT_UnderlyingString
%endmacro

;to define if true, 2 = string, 3 = part string
%macro HasPartStr 3
	%strlen HPS_PartStrlen %3
	%strlen HPS_WholeStrlen %2

	%assign HPS_counter 1

	%if HPS_WholeStrlen > HPS_PartStrlen
		%rep (HPS_WholeStrlen-HPS_PartStrlen)
			%substr HPS_TestStr %2 HPS_counter,HPS_PartStrlen	
			MGR_strcmp HPS_TestStr, %3,HPS_IS_EQUAL
			%ifdef HPS_IS_EQUAL
				%xdefine %1
				%undef HPS_IS_EQUAL
			%endif
			%assign HPS_counter (HPS_counter+1)
		%endrep
	%endif
%endmacro

%macro force_call 1
	%unmacro call 1
	call %1
	%macro call 1
		%error normal calls are for security reasons and type safety disabled please use force_call if you really want to call with the unsafe method
	%endmacro
%endmacro

%macro call 1
	%error normal calls are for security reasons and type safety disabled please use force_call if you really want to call with the unsafe method
%endmacro

; 1 = function name, 2 = parameter count
%macro CheckParameterCount 2
	%ifndef %1_CALLING_CONVENTION
		%error %1
		%error Unknown function specification, please specify the function by calling DEFINE_CALL
	%else
		%assign PAR_CNT %2
		%ifndef %1_VARIABLE_CALL
			%ifn %2 = %1_PARAMETER_COUNT
				%error The function %1 expects %1_PARAMETER_COUNT parameters not PAR_CNT
			%endif
		%else
			%ifn %2 > (%1_PARAMETER_COUNT-1)
				%error The function %1 takes at least %1_PARAMETER_COUNT parameters not only PAR_CNT
			%endif
		%endif
		%undef PAR_CNT
	%endif
%endmacro

;1 = function name, 2 = parameter count, 3 = calling convention
%macro DEFINE_CALL 2-3 STDCALL_VX64GCC
	%xdefine %1_CALLING_CONVENTION %3
	%xdefine %1_PARAMETER_COUNT %2
	%undef %1_VARIABLE_CALL
%endmacro

;a call which needs at leat %2 parameters
%macro DEFINE_VARIABLECALL 2-3 STDCALL_VX64GCC
	%xdefine %1_CALLING_CONVENTION %3
	%xdefine %1_PARAMETER_COUNT %2
	%xdefine %1_VARIABLE_CALL 1
%endmacro



;---------------------------------------------------------------------------

;1 = interface name, 2 = type_name, 3 = type offset, 4 = type length
%macro interface 4
	%ifnum %4
		%error The type length must be a structure, byte, word, dword, qword are defined as simple structures
	%else
		%ifdef %1_size
			%if %1_size < (%3+%4_size)
				%xdefine %1_size (%3+%4_size)
				%xdefine %1.%2 %3
				%xdefine %1_%2_STRUCTURE %4
			%else
				%xdefine %1.%2 %3
				%xdefine %1_%2_STRUCTURE %4
			%endif
			%assign %1_parameter (%1_parameter+1)
			RegisterByNum %1, %1_parameter, %2
		%else
			%xdefine %1_size (%3+%4_size)
			%xdefine %1.%2 %3
			%xdefine %1_%2_STRUCTURE %4
			%assign %1_parameter 1
			RegisterByNum %1, %1_parameter, %2
		%endif

	%endif
%endmacro

%macro interface_constructor 2
	%ifndef %1_size
		%error No interface with the name %1
	%else
		%ifdef %1_constructor
			%warning Overwriting the constructor of the interface %1
		%endif
		%xdefine %1_constructor %2
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
%macro UnregisterByNum 3
	InDepthUnregister %1, %1_%2, %3
	%undef %1_%2
%endmacro

%macro InDepthUnregister 3
	%undef %1.%2
	%ifdef %1_%2_STRUCTURE
		%undef %1_%2_STRUCTURE
	%endif
%endmacro

;1 = name
%macro uninterface 1
	%ifndef %1_parameter
		%error Unknown Interface
	%else
		%undef %1_size
		%undef %1_constructor
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
	InDepthUnregister %1, %2_%3, %2
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

%macro ReserveStackSpace 2
		InternReserveStackSpace %1, %2, %$STACK_NAME
%endmacro


%macro ReserveStackSpace 3+
		%defstr RSSArgList %3
		InternReserveStackSpace %1, %2, %$STACK_NAME, RSSArgList
		%undef RSSArgList
%endmacro

%macro InternReserveStackSpace 3-4
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
			
				%ifn %0 = 3
					%deftok IRSSTOKENIZE %4
					DefineNewStr %$%1_CONSTRUCTOR_LIST, IRSSTOKENIZE
					%undef IRSSTOKENIZE
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
		%ifn %2_PARAMETER_COUNT = 1
			%error The destructor function %2 must only take 1 parameter but takes %2_PARAMETER_COUNT instead
		%else
			%2_CALLING_CONVENTION %2,rbx
		%endif
	%else
		%error Trying to call function %2 without verifyed parameter count or calling convention
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

;1 = To define is equal, 2 = count, 
%macro IfCountEqual 2-*
	%if (%0-2) = %2
		%define %1
	%endif
%endmacro

%macro InDepthCall 4+
	mov rbx, rbp
	sub rbx, %1
	
	%ifdef %2_CALLING_CONVENTION
		IfCountEqual IS_EQUAL_NOW, %2_PARAMETER_COUNT-1, %4
		%ifdef IS_EQUAL_NOW
			%undef IS_EQUAL_NOW
			%if %2_PARAMETER_COUNT > 1
				%2_CALLING_CONVENTION %2,rbx,%4
			%else
				%2_CALLING_CONVENTION %2,rbx
			%endif
		%else
			%assign IDC_PAR_CNT (%0-3)
			%error The constructor function %2 expects %2_PARAMETER_COUNT instead of IDC_PAR_CNT arguments
			%undef IDC_PAR_CNT
		%endif
	%else
		%error Trying to call function without verifyed parameter count or calling convention
	%endif
%endmacro

;1 = string, 2 = terminated count, 3 = strname, 4 = str2name
%macro DivideStringsByChar 4
	%xdefine DSBC_counter 1
	%strlen DSBC_strlen %1
	%xdefine DSBC_is_sth 0
	
	%rep DSBC_strlen
		%if DSBC_is_sth = 0
			%substr DSBC_current_char %1 DSBC_counter
			%if DSBC_current_char = %2
				%substr %3 %1 1, (DSBC_counter-1)
				%substr %4 %1 (DSBC_counter+1), DSBC_strlen
				%xdefine DSBC_is_sth 1
				
			%endif
			%xdefine DSBC_counter (DSBC_counter+1)
		%endif
	%endrep
	
	%if DSBC_is_sth = 0
		%xdefine %3 %1
	%endif
	
	%undef DSBC_is_sth

	%undef DSBC_counter
	%undef DSBC_strlen
	%undef DSBC_current_char
%endmacro

%macro DSBC_IDD 3
	%xdefine %1%2 %3
%endmacro

%macro DivideStringsByChar 3
	%assign DSBC_counter 1
	%strlen DSBC_strlen %1
	%assign DSBC_is_sth 0

	%assign DSBC_strStart 1
	
	%rep DSBC_strlen
		%substr DSBC_current_char %1 DSBC_counter
		%if DSBC_current_char = %2
			%substr DSBC_SBSTR %1 DSBC_strStart, (DSBC_counter-DSBC_strStart)
			DSBC_IDD %3,DSBC_is_sth,DSBC_SBSTR
			%assign DSBC_strStart (DSBC_counter+1)	
			%assign DSBC_is_sth (DSBC_is_sth+1)	
		%endif
		%assign DSBC_counter (DSBC_counter+1)		
	%endrep
	
	%substr DSBC_SBSTR %1 DSBC_strStart, DSBC_strlen
	DSBC_IDD %3, DSBC_is_sth, DSBC_SBSTR

	%undef DSBC_SBSTR
	%undef DSBC_strStart
	
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

%macro CheckUpdateStackPtr 0
	InternCheckUpdateStackPtr %$STACK_NAME
%endmacro

%macro InternCheckUpdateStackPtr 1
	%ifn STACK_SPACE_%1_CURRENT = STACK_SPACE_%1_FLOATING
		%error moving data before updating the stack pointer is very risky!!
	%endif
%endmacro

%macro movzx 2
	%ifdef DISABLE_INST_MACRO_MOV
		movzx %1, %2
	%else
		%if NUM_STACKS_OPEN = 0
			movzx %1, %2
		%else
			CheckUpdateStackPtr
			FindAndReplaceLocalVar %1, OP0
			%ifdef FARLV_LOAD_EFFECTIVE_ADDRESS
				%undef FARLV_LOAD_EFFECTIVE_ADDRESS
				%error Cannot rewrite a fixed address!
			%else
				FindAndReplaceLocalVar %2, OP1
				%ifdef FARLV_LOAD_EFFECTIVE_ADDRESS
					%error cannot load zero extended effective address	
					%undef FARLV_LOAD_EFFECTIVE_ADDRESS
				%else
					movzx OP0, OP1
				%endif
				
				%undef OP0
				%undef OP1
			%endif
			%undef OP0
		%endif
	%endif
%endmacro

%macro mov 2
	%ifdef DISABLE_INST_MACRO_MOV
		mov %1, %2
	%else
		%if NUM_STACKS_OPEN = 0
			mov %1, %2
		%else
			CheckUpdateStackPtr
			FindAndReplaceLocalVar %1, OP0
			%ifdef FARLV_LOAD_EFFECTIVE_ADDRESS
				%undef FARLV_LOAD_EFFECTIVE_ADDRESS
				%error Cannot rewrite a fixed address!
			%else
				FindAndReplaceLocalVar %2, OP1
				%ifdef FARLV_LOAD_EFFECTIVE_ADDRESS
					%ifn OP0_SIZE < 8
						mov OP0, rbp
						sub OP0, OP1	
					%else
						%error %1 is only of size OP0_SIZE but needs to be 8 bytes large to hold an effective address
						%error mov OP0, rbp 	World overwrite sensitive data
					%endif
					%undef FARLV_LOAD_EFFECTIVE_ADDRESS
				%else
					%ifndef OP1_SIZE
						%xdefine OP1_SIZE 0
					%endif
					%if OP0_SIZE >= OP1_SIZE
						mov OP0, OP1
					%else
						%error %1 is only of size OP0_SIZE but %2 is of size OP1_SIZE
						%error mov %1, %2 Would overwrite sensitive data!
					%endif
				%endif
				
				%undef OP0
				%undef OP1
			%endif
			%undef OP0
			%undef OP1
			%undef OP0_SIZE
			%undef OP1_SIZE
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
%macro ILOOP_DEFINER 2
	%deftok FARLV_ending %1
	%xdefine FARLV_together FARLV_nextLink %+ . %+ FARLV_ending
	%strcat FARLV_offsetstr FARLV_offsetstr,'+'	
	%defstr FARLV_newOffset FARLV_together
	%strcat FARLV_offsetstr FARLV_offsetstr, FARLV_newOffset
	%xdefine FARLV_nextLink FARLV_nextLink %+ _ %+ FARLV_ending %+ _STRUCTURE
	%undef FARLV_newOffset
	%undef FARLV_together
%endmacro

;1=name of to define, 2=string
%macro CutAllWhiteSpace 2
	%assign CAWS_counter 1
	%strlen CAWS_strlen %2
	%xdefine %1 ''
	%rep CAWS_strlen
		%substr CAWS_char %2 CAWS_counter
		%ifn CAWS_char = ' '
			%strcat %1 %1, CAWS_char	
		%endif
		%assign CAWS_counter (CAWS_counter+1)
	%endrep

	%undef CAWS_counter
	%undef CAWS_strlen
	%undef CAWS_char
%endmacro

;1 = name of to define if true, 2 = first string, 3 = secStr
%macro MultipleStrCmp 3-*
	%xdefine MSC_TODEFINE %1
	%xdefine MSC_FRSTSTR %2

	%rep (%0-2)
		%ifstr %3
			MGR_strcmp MSC_FRSTSTR, %3, MSC_IS_EQUAL
			%ifdef MSC_IS_EQUAL
				%undef MSC_IS_EQUAL
				DepthDefine MSC_TODEFINE
			%endif
		%endif
		%rotate 1
	%endrep
	%undef MSC_TODEFINE
	%undef MSC_FRSTSTR
%endmacro

%macro DepthDefine 1
	%xdefine %1
%endmacro
							
;1 = expression, 2 = Fixed Token name
%macro FindAndReplaceLocalVar 2
	%xdefine %2 %1
	%defstr FARLV_KlopStr %1
	CutAllWhiteSpace UnderlyingStr, FARLV_KlopStr
	%undef FARLV_KlopStr
	%undef %2_SIZE
	DeductType %2_SIZE, UnderlyingStr
	
	%assign FARLV_counter 1
	%assign FARLV_counter_end 1
	%strlen FARLV_strlen UnderlyingStr

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
		%elif FARLV_counter_end = FARLV_strlen
			%xdefine FARLV_SUSPECT
			%assign FARLV_counter_end (FARLV_counter_end+1)
		%endif

		%ifdef FARLV_SUSPECT	
			%undef FARLV_SUSPECT
			%substr FARLV_varstr UnderlyingStr FARLV_counter, (FARLV_counter_end-FARLV_counter)
	
			%xdefine FARLV_DeductType FARLV_varstr
			%strcat FARLV_DeductType FARLV_DeductType,FARLV_character

			MGR_strcmp FARLV_DeductType, 'byte[', FARLV_MEM_BYTE		
			MGR_strcmp FARLV_DeductType, 'word[', FARLV_MEM_WORD
			MGR_strcmp FARLV_DeductType, 'dword[', FARLV_MEM_DWORD
			MGR_strcmp FARLV_DeductType, 'qword[', FARLV_MEM_QWORD

			%ifdef FARLV_MEM_BYTE
				%undef FARLV_MEM_BYTE
				%xdefine FARLV_MEM_ACCESS 1
				%xdefine FARLV_MEM_ACCESS_STR byte
			%elifdef FARLV_MEM_WORD
				%undef FARLV_MEM_WORD
				%xdefine FARLV_MEM_ACCESS 2
				%xdefine FARLV_MEM_ACCESS_STR word
			%elifdef FARLV_MEM_DWORD
				%undef FARLV_MEM_DWORD
				%xdefine FARLV_MEM_ACCESS 4
				%xdefine FARLV_MEM_ACCESS_STR dword
			%elifdef FARLV_MEM_QWORD
				%undef FARLV_MEM_QWORD
				%xdefine FARLV_MEM_ACCESS 8
				%xdefine FARLV_MEM_ACCESS_STR qword
			%endif

			%undef FARLV_DeductType


			%undef FARLV_STR0
			%undef FARLV_STR1
			%undef FARLV_STR2
			%undef FARLV_STR3
			%undef FARLV_STR4
			%undef FARLV_STR5
			%undef FARLV_STR6
			%undef FARLV_STR7
			%undef FARLV_STR8
			%undef FARLV_STR9
			%undef FARLV_STR10
			%undef FARLV_STR11
			%undef FARLV_STR12
			%undef FARLV_STR13
			%undef FARLV_STR14
			%undef FARLV_STR15
			%undef FARLV_STR16
			%undef FARLV_STR17
			%undef FARLV_STR18	;Enable recursion of strings till depth of 18
			
			DivideStringsByChar FARLV_varstr,'.',FARLV_STR 
		
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
					%xdefine FARLV_nextLink FARLV_beg %+ _ %+ FARLV_ending %+ _STRUCTURE
					%defstr FARLV_offsetstr FARLV_together
					%strcat FARLV_offsetstr '(', FARLV_offsetstr

					%assign ILOOP_counter 2
					%assign ILOOP_link 3
					
					%rep 15
						%xdefine FARLV_checkDefine 'FARLV_STR'
						%defstr FARLV_checkDefineVar ILOOP_counter
						%strcat FARLV_checkDefine FARLV_checkDefine, FARLV_checkDefineVar
					
						IsLocalDefined VAR_EXISTS, FARLV_checkDefine

						%ifdef VAR_EXISTS
							ILOOP_DEFINER FARLV_STR %+ ILOOP_counter, FARLV_STR %+ ILOOP_link
							
						
							%assign ILOOP_counter (ILOOP_counter+1)
							%assign ILOOP_link (ILOOP_link+1)
						%endif
						%undef VAR_EXISTS
					%endrep

					%xdefine FARLV_DEDUCT_TYPE FARLV_nextLink %+ _size
					%ifdef FARLV_MEM_ACCESS
						%if FARLV_DEDUCT_TYPE < FARLV_MEM_ACCESS
							%error Trying to access a FARLV_nextLink variable as like a FARLV_MEM_ACCESS_STR variable
						%endif
					%endif
	
	
					%undef FARLV_checkDefine
					%undef FARLV_checkDefineVar
					%undef ILOOP_counter
					%undef ILOOP_link


					%strcat FARLV_var FARLV_var, '-'
					%strcat FARLV_var FARLV_var, FARLV_offsetstr
					%strcat FARLV_var FARLV_var,')'
		
					%undef FARLV_nextLink	
					%undef FARLV_ending
					%undef FARLV_STR1
					%undef FARLV_beg
					%undef FARLV_together
					%undef FARLV_offsetstr
				%endif			

				%deftok FARLV_realoffset FARLV_var

				%ifdef %2_SIZE
					%xdefine FARLV_MEM_ACCESS %2_SIZE
					%xdefine %2_SIZE FARLV_DEDUCT_TYPE
				%endif				

				%ifdef FARLV_MEM_ACCESS
					%xdefine FARLV_complete rbp - (FARLV_realoffset)
					%defstr FARLV_completestr FARLV_complete
					%substr FARLV_ENDSTR UnderlyingStr 1,(FARLV_counter-1)
					%strcat FARLV_ENDSTR FARLV_ENDSTR, FARLV_completestr
					%substr FARLV_endStr UnderlyingStr (FARLV_counter_end),((FARLV_strlen+1)-FARLV_counter_end)
					%strcat FARLV_ENDSTR FARLV_ENDSTR, FARLV_endStr
					%deftok %2 FARLV_ENDSTR
				%else

					%xdefine %2 FARLV_realoffset
					%xdefine FARLV_LOAD_EFFECTIVE_ADDRESS
				
				%endif
					
				%undef FARLV_DEDUCT_TYPE
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
	%undef FARLV_MEM_ACCESS
	%undef FARLV_MEM_ACCESS_STR
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
