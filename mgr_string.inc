%ifndef _MORGENROETE_STRING_INC_
%define _MORGENROETE_STRING_INC_

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

;1 = name of to define if true, 2 = first string, 3 = secStr
%macro MultipleStrCmp 3-*
	%xdefine MSC_TODEFINE %1
	%xdefine MSC_FRSTSTR %2

	%rep (%0-2)
		%ifstr %3
			MGRStrcmp MSC_FRSTSTR, %3, MSC_IS_EQUAL
			%ifdef MSC_IS_EQUAL
				%undef MSC_IS_EQUAL
				%macro DepthDefine 1
					%xdefine %1
				%endmacro
				DepthDefine MSC_TODEFINE
				%unmacro DepthDefine 1
			%endif
		%endif
		%rotate 1
	%endrep
	%undef MSC_TODEFINE
	%undef MSC_FRSTSTR
%endmacro

;to define if true, 2 = string, 3 = part string
%macro HasPartStr 3
	%strlen HPS_PartStrlen %3
	%strlen HPS_WholeStrlen %2

	%assign HPS_counter 1

	%if HPS_WholeStrlen > HPS_PartStrlen
		%rep ((HPS_WholeStrlen+1)-HPS_PartStrlen)
			%substr HPS_TestStr %2 HPS_counter,HPS_PartStrlen	
			MGRStrcmp HPS_TestStr, %3,HPS_IS_EQUAL
			%ifdef HPS_IS_EQUAL
				%xdefine %1
				%undef HPS_IS_EQUAL
			%endif
			%assign HPS_counter (HPS_counter+1)
		%endrep
	%endif

	%undef HPS_PartStrlen
	%undef HPS_WholeStrlen
	%undef HPS_counter
	%undef HPS_TestStr
%endmacro

;1 = new defined string, 2 = string to replace, 3 = replacment, 4 = replacement str, 5 = number of replacments
%macro SearchAndReplace 5
	%strlen SAR_Strlen %2
	%strlen SAR_StrlenRep %3
	%assign %5 0
	%xdefine %1 ''
	
	%assign SAR_Counter 1

	%rep SAR_Strlen
		%substr SAR_SubStr %2 SAR_Counter, SAR_StrlenRep
		%if SAR_SubStr = %3
			%strcat %1 %1, %4
			%assign %5 (%5+1)
			%assign SAR_Counter (SAR_Counter+(SAR_StrlenRep-1))
		%else
			%substr SAR_SubStr %2 SAR_Counter
			%strcat %1 %1, SAR_SubStr
		%endif

		%assign SAR_Counter (SAR_Counter+1)
	%endrep

	%undef SAR_Strlen
	%undef SAR_StrlenRep
	%undef SAR_Counter
	%undef SAR_SubStr
%endmacro

;1 = string 1, 2 = string 2, 3 =defined if equal
%macro MGRStrcmp 3
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


%macro DivideStringsByChar_IDD 3
	%xdefine %1%2 %3
%endmacro

;1=name of to define, 2=string, 3 = character to cut
%macro CutAllCharactersOfType 3
	%assign CAWS_counter 1
	%strlen CAWS_strlen %2
	%xdefine %1 ''
	%rep CAWS_strlen
		%substr CAWS_char %2 CAWS_counter
		%ifn CAWS_char = %3
			%strcat %1 %1, CAWS_char	
		%endif
		%assign CAWS_counter (CAWS_counter+1)
	%endrep

	%undef CAWS_counter
	%undef CAWS_strlen
	%undef CAWS_char
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

;1 = divideable string, 2 = char which will indicate string parting, 3 = Base Token which will get the appended num, 4 = number of strings defined
%macro DivideStringsByChar 4
	%assign DSBC_counter 1
	%strlen DSBC_strlen %1
	%assign DSBC_is_sth 0

	%assign DSBC_strStart 1
	
	%rep DSBC_strlen
		%substr DSBC_current_char %1 DSBC_counter
		%if DSBC_current_char = %2
			%substr DSBC_SBSTR %1 DSBC_strStart, (DSBC_counter-DSBC_strStart)
			DivideStringsByChar_IDD %3,DSBC_is_sth,DSBC_SBSTR
			%assign DSBC_strStart (DSBC_counter+1)	
			%assign DSBC_is_sth (DSBC_is_sth+1)	
		%endif
		%assign DSBC_counter (DSBC_counter+1)		
	%endrep
	
	%substr DSBC_SBSTR %1 DSBC_strStart, DSBC_strlen
	DivideStringsByChar_IDD %3, DSBC_is_sth, DSBC_SBSTR

	%assign %4 (DSBC_is_sth+1)

	%undef DSBC_SBSTR
	%undef DSBC_strStart
	
	%undef DSBC_is_sth

	%undef DSBC_counter
	%undef DSBC_strlen
	%undef DSBC_current_char
%endmacro


%macro PartStringsByChar 4
	%xdefine DSBC_counter 1
	%strlen DSBC_strlen %1
	%strlen DSBC_TokStrlen %2
	%xdefine DSBC_is_sth 0
	
	%rep DSBC_strlen
		%if DSBC_is_sth = 0
			%substr DSBC_current_char %1 DSBC_counter, DSBC_TokStrlen
			%if DSBC_current_char = %2
				%substr %3 %1 1, (DSBC_counter-1)
				%substr %4 %1 (DSBC_counter+DSBC_TokStrlen), DSBC_strlen
				%xdefine DSBC_is_sth 1
			%endif
			%xdefine DSBC_counter (DSBC_counter+1)
		%endif
	%endrep
	
	%if DSBC_is_sth = 0
		%xdefine %3 %1
	%endif

	%undef DSBC_TokStrlen
	%undef DSBC_is_sth

	%undef DSBC_counter
	%undef DSBC_strlen
	%undef DSBC_current_char
%endmacro

;1 = string, 2 = to search for in string, 3 = to replace in string, 4 = new string define
%macro ReplaceInStringOnly 4
	%strlen RISO_Strlen %1
	%strlen RISO_RepStr %2

	%assign RISO_Counter 1

	%xdefine %4 ''
	%undef RISO_InStr

	%rep RISO_Strlen

		%ifdef RISO_InStr
			%substr RISO_SubStr %1 RISO_Counter, RISO_RepStr
			MGRStrcmp RISO_SubStr, %2, RISO_ICtr
			%ifdef RISO_ICtr
				%strcat %4 %4, %3	
				%undef RISO_ICtr
				%assign RISO_Counter (RISO_Counter + RISO_RepStr-1)
			%else
				%substr RISO_char %1 RISO_Counter

				
				%if RISO_char = RISO_InStr
					%undef RISO_InStr
				%endif

				%if RISO_char = "'"
					%strcat %4 %4, '"'
				%else
					%strcat %4 %4, RISO_char
				%endif
			%endif
		%else
			%substr RISO_char %1 RISO_Counter
			

			%if RISO_char = '"'
				%xdefine RISO_InStr '"'	
			%elif RISO_char = "'"
				%xdefine RISO_InStr "'"
				%xdefine RISO_char '"'
			%elif RISO_char = '{'
				%xdefine RISO_InStr '}'
			%endif

			%strcat %4 %4, RISO_char
		%endif

		%assign RISO_Counter (RISO_Counter+1)
	%endrep

	%undef RISO_InStr
	%undef RISO_ICtr
	%undef RISO_char
	%undef RISO_Counter
	%undef RISO_Strlen
	%undef RISO_RepStr
	%undef RISO_SubStr
%endmacro




%macro PartStringsByCharLast 4
	%xdefine DSBC_counter 1
	%strlen DSBC_strlen %1
	%strlen DSBC_TokStrlen %2
	%xdefine DSBC_is_sth 1
	
	%rep DSBC_strlen
		%substr DSBC_current_char %1 DSBC_counter, DSBC_TokStrlen
		%if DSBC_current_char = %2
			%xdefine DSBC_is_sth DSBC_counter
		%endif
		%xdefine DSBC_counter (DSBC_counter+1)
	%endrep
	
	%if DSBC_is_sth = 1
		%xdefine %4 %1
	%else
		%substr %3 %1 1,(DSBC_is_sth-1)
		%substr %4 %1 (DSBC_is_sth+DSBC_TokStrlen), ((DSBC_strlen+1)-(DSBC_is_sth+DSBC_TokStrlen))
	%endif

	%undef DSBC_TokStrlen
	%undef DSBC_is_sth

	%undef DSBC_counter
	%undef DSBC_strlen
	%undef DSBC_current_char
%endmacro


%endif
