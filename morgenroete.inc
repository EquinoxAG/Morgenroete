%ifndef _MORGENROETE_MASTER_FILE_
%define _MORGENROETE_MASTER_FILE_

;1 = interface name, 2 = type_name, 3 = type offset, 4 = type length
%macro interface 4
	%ifdef %1_size
		%if %1_size < (%3+%4)
			%xdefine %1_size (%3+%4)
			%define %1.%2 %3
			%define %1_%2_LENGTH %4
		%else
			%define %1.%2 %3
			%define %1_%2_LENGTH %4
		%endif
	%else
		%xdefine %1_size (%3+%4)
		%define %1.%2 %3
		%define %1_%2_LENGTH %4
	%endif
%endmacro


%endif
