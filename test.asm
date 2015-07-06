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

interface_open multiboot
	add mb2, multiboot2
interface_close

interface_destructor multiboot, MultibootDst
interface_constructor multiboot, InitialiseVGADriver 

%define MB_ADDR 0x1000


DefineFunction InitialiseVGADriver, 2, 'hello.asm'

DefineClassFunction HelloWorld, 2, multiboot,'test.asm', STDCALL_GCC64

DefineFunction MultibootDst, 1, 'hello.asm'

DeclareFunction main, InternDef, GlobalDef, DarkSide
	special_call ((yolo->multiboot).col->darlsode).HelloWord()
EndFunction

MyIntok:
	dq 0, 0, 0
