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


DefineFunction HelloWorld, 1
DeclareFunction HelloWord(strss)
	jmp $
EndFunction


DeclareFunction main(Dolo,Solo)
	secure_call HelloWorld("Hallo Killas")
EndFunction

MyIntok:
	dq 0, 0, 0


ImportAllMgrFunctions
