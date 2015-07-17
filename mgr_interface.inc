%ifndef _MORGENROETE_INTERFACE_INC_
%define _MORGENROETE_INTERFACE_INC_

%include "mgr_string.inc"

;Define some basic sizes
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


;1 = interface name, 2 = type_name, 3 = type offset, 4 = type length
%macro MGRInternInterface 4
	%ifnum %4
		%error The type length must be a structure, byte, word, dword, qword are defined as simple structures
	%else
		%ifndef %4_size
			%eror The structure %4 must have a fixed size called %4_size
		%else
			%macro RegisterInterfaceAttributeByNum 3
				%xdefine %1_%2 %3
			%endmacro

			%ifdef %1_size
				%if %1_size < (%3+%4_size)
					%xdefine %1_size (%3+%4_size)	;Resize the structure size
					%xdefine %1.%2 %3		;Define specific member
					%xdefine %1_%2_STRUCTURE %4	;Define specific members type
				%else
					%xdefine %1.%2 %3		;No need to resize the structure
					%xdefine %1_%2_STRUCTURE %4
				%endif
				%assign %1_parameter (%1_parameter+1)
				RegisterInterfaceAttributeByNum %1, %1_parameter, %2				
			%else
				%xdefine %1_size (%3+%4_size)
				%xdefine %1.%2 %3
				%xdefine %1_%2_STRUCTURE %4
				%assign %1_parameter 1
				RegisterInterfaceAttributeByNum %1, %1_parameter, %2
				%ifndef %1_base_classes
					%xdefine %1_base_classes %1
					%xdefine %1_num_base_classes 1
				%endif
			%endif
		
			%unmacro RegisterInterfaceAttributeByNum 3
		%endif
	%endif
%endmacro

; 1 = interface name, 2 = attribute name, 3 = attribute struc
%macro MGRInternInterface 3
	%ifdef %1_size
		MGRInternInterface %1, %2, %1_size, %3
	%else
		MGRInternInterface %1, %2, 0, %3
	%endif
%endmacro


%macro interface_open 1
	%ifdef MGR_CURRENT_INTERFACE
		%error There can only be one interface opened at a time please use interface_close before
	%else

		%ifdef %1_parameter
			%fatal Syntax error cannot open a already defined interface
		%else
			%xdefine MGR_CURRENT_INTERFACE %1
			%macro add 2
				MGRInternInterface MGR_CURRENT_INTERFACE, %1, %2
			%endmacro
			%macro add 3
				MGRInternInterface MGR_CURRENT_INTERFACE, %1, %2, %3
			%endmacro
		%endif
	%endif
%endmacro

%macro interface_close 0
	%ifdef MGR_CURRENT_INTERFACE
		%undef MGR_CURRENT_INTERFACE
		%unmacro add 2
		%unmacro add 3
	%else
		%error Currently there is no interface open
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

		%assign MGR_UNINTERFACE_counter 1
		%rep %1_parameter
			UnregisterInterfaceAttributeByNum %1, MGR_UNINTERFACE_counter
			%assign MGR_UNINTERFACE_counter (MGR_UNINTERFACE_counter+1)
		%endrep
		%undef MGR_UNINTERFACE_counter
		
		%undef %1_parameter
	%endif
%endmacro

;1 = interface name, 2 = to define the number
%macro GetInterfaceNumBaseClasses 2
	%xdefine %2 %1_num_base_classes
%endmacro

;1 = interface name, 2 = base class number, 3 = to define the base class
%macro GetBaseClassByNum 3
	%macro DeepDefineBaseClass 2-*
		%xdefine To_Def_Name %1
		%xdefine Base_Class_Num %2

		%rep (Base_Class_Num+1)
			%rotate 1
		%endrep

		%macro VeryDeppDefine 2
			%error Define %1 to %2
			%xdefine %1 %2
		%endmacro
		VeryDeppDefine To_Def_Name, %1
		%unmacro VeryDeppDefine 2
	%endmacro

	DeepDefineBaseClass %3, %2, %1_base_classes

	%unmacro DeepDefineBaseClass 2-*
%endmacro


;1 to define interface name, 2 to get from interface name, 3 = number
%macro CloneAttributeByNum 3
	%?? %1, %2, %3, %2_%3
%endmacro

%macro CloneAttributeByNum 4
	%xdefine %1.%4 %2.%4
	%xdefine %1_%4_STRUCTURE %2_%4_STRUCTURE
%endmacro


%macro DeriveInterface 1
	%ifdef MGR_CURRENT_INTERFACE
		MGRInternDeriveInterface MGR_CURRENT_INTERFACE, %1
	%else
		%error There is no interface openend at the moment
	%endif
%endmacro
;1 = name of inheriting, name of to inherit from
%macro MGRInternDeriveInterface 2
	%ifdef %1_size
		%error Only an empty interface can inherit attributes!
	%elifndef %2_size
		%error Unknown interface %2, cannot build derivation tree
	%else
		%xdefine %1_size %2_size
		%xdefine %1_base_classes %1 %+ , %+ %2_base_classes
		%xdefine %1_num_base_classes (%2_num_base_classes+1)
		%assign %1_parameter %2_parameter
		%assign DERIVE_INTERFACE_counter 1
		%rep %2_parameter
			CloneAttributeByNum %1, %2, DERIVE_INTERFACE_counter
			%assign DERIVE_INTERFACE_counter (DERIVE_INTERFACE_counter+1)	
		%endrep
	%endif
%endmacro

%macro UnregisterInterfaceAttributeByNum 2
	%?? %1, %2, %1_%2		; Unregister attribute and attribute assigned structure
	%undef %1_%2			; Unregister the parameter itself
%endmacro

%macro UnregisterInterfaceAttributeByNum 3
	%undef %1.%3
	%undef %1_%3_STRUCTURE
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

;1 = Heap variable name, 2 = interface name
%macro ConnectHeapToInterface 2
	%ifdef %1_MGR_CONNECTED_INTERFACE
		%warning Disconnecting the heap %1 from the currently connected interface %1_MGR_CONNECTED_INTERFACE
		UnconnectHeapFromInterface %1
		%xdefine %1_MGR_CONNECTED_INTERFACE %2
	%else
		%xdefine %1_MGR_CONNECTED_INTERFACE %2
	%endif
%endmacro

;1 = heap name
%macro UnconnectHeapFromInterface 1
	%ifndef %1_MGR_CONNECTED_INTERFACE
		%error Can not unconnect %1 from its interface. It is currently not connected to an interface
	%else
		%undef %1_MGR_CONNECTED_INTERFACE
	%endif
%endmacro

;1 = name of the interface, to define if interface
%macro IsInterface 2
	%strcat II_TryStr %1,'_parameter'
	%deftok II_TryTok II_TryStr
	%defstr II_castStr II_TryTok

	MGRStrcmp II_TryStr, II_castStr, %2

	%ifdef %2
		%undef %2
	%else
		%xdefine %2
	%endif

	%undef II_TryStr
	%undef II_castStr
	%undef II_TryTok
%endmacro

;1=string of possible variable;2=To define if defined
%macro IsConnectedToHeap 2
	%xdefine ISCTH_str %1
	%strcat ISCTH_str ISCTH_str, '_MGR_CONNECTED_INTERFACE'
	%deftok ISCTH_tok ISCTH_str	
	%defstr ISCTH_tokstr ISCTH_tok
	MGRStrcmp ISCTH_str, ISCTH_tokstr, %2

	%ifndef %2
		%xdefine %2 ISCTH_tok
	%else
		%undef %2
	%endif
	
	%undef ISCTH_str
	%undef ISCTH_tok
	%undef ISCTH_tokstr
%endmacro

; 1 = string of interface = "tod.register.dark", 2 = Structure of invisible token, 3 = string to define the offset, 4 = token to define the structure of the 
%macro TraverseInterfaceChain 4
	DivideStringsByChar %1, '.', TraverseInterfaceChain_Member, TraverseInterfaceChain_NumInterfaces
	%xdefine TraverseInterfaceChain_totaloffset '('
	%xdefine TraverseInterfaceChain_currentStruc %2
	%assign TraverseInterfaceChain_counter 0

	%rep TraverseInterfaceChain_NumInterfaces
		%xdefine TIC_CurrentMember TraverseInterfaceChain_Member %+ TraverseInterfaceChain_counter
		HasPartStr TIC_CastStructure, TIC_CurrentMember, '->'
		%ifdef TIC_CastStructure
			%undef TIC_CastStructure
			PartStringsByChar TIC_CurrentMember, '->', TIC_RealCurrentMember, TIC_CurrentStructure
			%xdefine TIC_CurrentMember TIC_RealCurrentMember
		
			%undef TIC_RealCurrentMember
		%endif
		%deftok TIC_CurrentMemberTok TIC_CurrentMember


		%macro TIC_IsDefined 2
			%ifndef %1.%2
				%fatal The interface %1 has not got a member named %2
			%endif
		%endmacro
		TIC_IsDefined TraverseInterfaceChain_currentStruc, TIC_CurrentMemberTok
		%unmacro TIC_IsDefined 2
		%xdefine TIC_CurrentOffset TraverseInterfaceChain_currentStruc %+ . %+ TIC_CurrentMemberTok


		%defstr TIC_CurrentOffsetStr TIC_CurrentOffset
	
		%strlen TIC_totaloffsetlen TraverseInterfaceChain_totaloffset	
		%ifn TIC_totaloffsetlen = 1
			%strcat TraverseInterfaceChain_totaloffset TraverseInterfaceChain_totaloffset, '+'
		%endif
		

		%strcat TraverseInterfaceChain_totaloffset TraverseInterfaceChain_totaloffset, TIC_CurrentOffsetStr

		%ifndef TIC_CurrentStructure
			%xdefine TraverseInterfaceChain_currentStruc TraverseInterfaceChain_currentStruc %+ _ %+ TIC_CurrentMemberTok %+ _STRUCTURE
		%else
			%deftok TraverseInterfaceChain_currentStruc TIC_CurrentStructure
			%undef TIC_CurrentStructure
		%endif

		
		%defstr TIC_CrrStr TraverseInterfaceChain_currentStruc
		MultipleStrCmp TIC_IsLowType, TIC_CrrStr, 'qword', 'dword', 'word', 'byte'

		%ifndef TIC_IsLowType
			IsInterface TIC_CrrStr, TIC_IsItInterface

			%ifdef TIC_IsItInterface
				%undef TIC_IsItInterface
			%else
				%error Not recognized interface! TIC_CrrStr
				%xdefine TraverseInterfaceChain_currentStruc void 
				%xdefine void_size 0
			%endif
		%else
			%undef TIC_IsLowType
		%endif

		%undef TIC_CrrStr

		%macro TIC_HelperFunc 2
			%undef %1%2
		%endmacro	
		TIC_HelperFunc TraverseInterfaceChain_Member,TraverseInterfaceChain_counter
		%unmacro TIC_HelperFunc 2
		
		%assign TraverseInterfaceChain_counter (TraverseInterfaceChain_counter+1)
	%endrep	
	%strcat TraverseInterfaceChain_totaloffset TraverseInterfaceChain_totaloffset, ')'

	%xdefine %3 TraverseInterfaceChain_totaloffset
	%xdefine %4 TraverseInterfaceChain_currentStruc


	%undef TIC_CurrentMemberTok
	%undef TIC_CurrentOffset
	%undef TIC_CurrentOffsetStr
	%undef TIC_CurrentMember
	%undef TIC_totaloffsetlen	

	%undef TraverseInterfaceChain_NumInterfaces
	%undef TraverseInterfaceChain_totaloffset
	%undef TraverseInterfaceChain_currentStruc
	%undef TraverseInterfaceChain_counter
%endmacro

%endif
