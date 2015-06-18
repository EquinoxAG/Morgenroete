%ifndef _MORGENROETE_MASTER_FILE_
%define _MORGENROETE_MASTER_FILE_


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
		RegisterByNum %1, %1_parameter, %2
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
	%endif
%endmacro

%macro ReserveStackSpace 2
	InternReserveStackSpace %1, %2, %$STACK_NAME
%endmacro

%macro InternReserveStackSpace 3
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
			
			%undef TryStr
		%else
			%error A variable with the name %1 already exists
			%undef VAR_EXISTS
		%endif
	%endif
%endmacro

%macro UpdateStackPtr 0
	InternUpdateStackPtr %$STACK_NAME
%endmacro

%macro InternUpdateStackPtr 1
	%ifn STACK_SPACE_%1_CURRENT = STACK_SPACE_%1_FLOATING
		sub rsp, (STACK_SPACE_%1_FLOATING - STACK_SPACE_%1_CURRENT)
		%xdefine STACK_SPACE_%1_CURRENT (STACK_SPACE_%1_FLOATING)
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
	%ifn TryStrILD = %2
		%xdefine %1 0
	%endif
	%undef TryStrILD
%endmacro

%macro DestroyStack 1
	%ifdef STACK_NAME_%1
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
