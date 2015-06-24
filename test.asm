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

DefineFunction HelloWorld, 2,'test.asm', STDCALL_GCC64

DefineFunction MultibootDst, 1, 'hello.asm'

DeclareFunction main, InternDef, GlobalDef, DarkSide
	mov rax, Arg_InternDef
	mov rdx, Arg_GlobalDef
	mov rax, Arg_DarkSide

	ReserveStackSpace MyVar, multiboot, 100
	UpdateStackPtr

	mov_ts qword[ MyVar.mb2 ], rax

	secure_call InitialiseVGADriver, 100, rax, rbx

	mov rdx, multiboot2.sig
EndFunction

MyIntok:
	dq 0, 0, 0
