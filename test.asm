%include "Morgenroetev1.inc"

interface_open multiboot
	add sig, qword
	add rev, dword 
interface_close


interface_open point
	add x_coord, qword
	add y_coord, qword
	add z_coord, dword
interface_close

interface_open multiboot2
	DeriveInterface multiboot
	add point3D, point
interface_close


%define MB_ADDR 0x1000


DefineFunction ShitHouse, 2+
DefineFunction multiboot::ShitHouse, 1
DefineFunction HelloWorld, 0

;interface_constructor multiboot, ShitHouse


bitmap_open MyBitmap
	add flags, 3,1
	add care, 2, 0
bitmap_close


%macro TestStr 1+
	%defstr SOLO %1
	%error Some place SOLO
	ReplaceInStringOnly SOLO, ',','MGR_RESET_COMMA',NEW_StrS
	%error Some place NEW_StrS
	ReplaceInStringOnly SOLO, 'MGR_RESET_COMMA', ',', NEW_SuperStr
	%error Some place NEW_SuperStr

%endmacro 

DeclareFunction main(Dolo,Solo)
	CreateStack Yolo
	ReserveStackSpace MyVar, multiboot
	UpdateStackPtr

	MyBitmap.flags.set(3)

	%assign VAL1 MyBitmap.flags.get( MGR_BMP_OFF )

	;ResoluteFunctionName multiboot::ShitHouse, 2+



	secure_call MyVar.ShitHouse({"Was los, yolo, solo",'Hangout'})


	DestroyStack Yolo
EndFunction

MyIntok:
	dq 0, 0, 0


ImportAllMgrFunctions
