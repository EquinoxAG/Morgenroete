%ifndef _MORGENROETE_BITMAP_INC_
%define _MORGENROETE_BITMAP_INC_


%xdefine MGR_BMP_VAL 1
%xdefine MGR_BMP_OFF 2
%xdefine MGR_BMP_MASK 4
%xdefine MGR_BMP_REVMASK 8

;1 = name of the bitmap
%macro bitmap_open 1
	%ifdef %1_size
		%fatal There is already a bitmap or a class with the name %1
	%else
		%ifnmacro add 2
			%assign %1_size 0
			%xdefine CURRENT_OPENED_BITMAP %1
			%assign %1_fullVal 0
			%define %1.get(x) ((%1_fullVal*(x&MGR_BMP_VAL)) + (0xFFFFFFFFFFFFFFFF*((x&MGR_BMP_MASK)>>2)))
			%xdefine %1.set(x) mgr_bitmap_set %1, x
			%xdefine %1.or(x) mgr_bitmap_set %1, x

			%macro add 2
				bitmap_add %1, %2
			%endmacro
			%macro add 3
				bitmap_add %1, %2, %3
			%endmacro

		%else
			%fatal There is already a interface opened at the moment, impossible to open a bitmap inside of it
		%endif
	%endif
%endmacro

%macro mgr_bitmap_set 2
	%assign %1_fullVal %2
%endmacro

%macro mgr_bitmap_or 2
	%assign %1_fullVal (%1_fullVal|%2)
%endmacro

;1 = name, 2 = bit length
%macro bitmap_add 2
	%xdefine BA_CurrSize CURRENT_OPENED_BITMAP %+ _size
	bitmap_add_intern %1, %2, BA_CurrSize, CURRENT_OPENED_BITMAP
	%undef BA_CurrSize
%endmacro

%macro bitmap_add 3
	bitmap_add_intern %1, %2, %3, CURRENT_OPENED_BITMAP
%endmacro

;1 = name, 2 = bit length, 3 = bit offset, 4 = bitmap name
%macro bitmap_add_intern 4
	%ifdef %4.%1_size
		%fatal There is already a member named %1 of the bitmap %4
	%else
		%if %2 = 0
			%fatal The member %4::%1 has a bit length of 0, that is impossible
		%endif

		%assign %4.%1_size %2
		%assign %4.%1_offset %3

		%assign %4.%1_mask 0xFFFFFFFFFFFFFFFF

		%assign BAI_Counter 1
		%rep %2
			%assign %4.%1_mask ((~(BAI_Counter<<%3))&%4.%1_mask)
			%assign BAI_Counter (BAI_Counter+1)
		%endrep
		%undef BAI_Counter


		%define %4.%1.get(x) ((((%4_fullVal&(~%4.%1_mask))>>%4.%1_offset)*(x&MGR_BMP_VAL))+(%4.%1_offset*((x&MGR_BMP_OFF)>>1))+((~%4.%1_mask)*((x&MGR_BMP_MASK)>>2))+(%4.%1_mask*((x&MGR_BMP_REVMASK)>>3)))
		%xdefine %4.%1.set(x) bitmap_set_val %4, %1, x
		%xdefine %4.%1.or(x) bitmap_or_val %4, %1, x
		%assign %4_size (%3+%2)

		%if %4_size > 64
			%fatal A Bitmap must not have a size above 64 bits
		%endif
	%endif
%endmacro

%macro bitmap_or_val 3
	%assign %1_fullVal (%1_fullVal|(%1.%2.get<<%1.%2_offset))
%endmacro

; 1 = interface name, 2 = name of the full member, 3 = value
%macro bitmap_set_val 3
	%assign %1_fullVal ((%1_fullVal&%1.%2_mask)|(%3<<%1.%2_offset))
%endmacro

%macro bitmap_close 0
	%ifdef CURRENT_OPENED_BITMAP
		%undef CURRENT_OPENED_BITMAP
		%unmacro add 2
		%unmacro add 3
	%else
		%fatal There is no bitmap open at the moment
	%endif
%endmacro

%endif
