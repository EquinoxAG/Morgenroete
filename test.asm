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
DefineFunction multiboot::ShitHouse, 2+
DefineFunction HelloWorld, 0



bitmap_open MyBitmap
	add flags, 3,1
	add care, 2, 0
bitmap_close

DeclareFunction Colo()
EndFunction

DeclareFunction main(Dolo,Solo)
	CreateStack Yolo
	ReserveStackSpace MyVar, multiboot2
	UpdateStackPtr

	MyBitmap.flags.set(3)
	MyBitmap.care.set(10b)

	%assign VAL1 MyBitmap.flags.get( MGR_BMP_OFF )

	secure_call MyVar.ShitHouse(rax,rbx,rcx, rsi)
	secure_call main(0,0)
	DestroyStack Yolo
EndFunction

MyIntok:
	dq 0, 0, 0


ImportAllMgrFunctions
