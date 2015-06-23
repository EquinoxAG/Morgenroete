%include "Morgenroetev1.inc"

interface_open multiboot
	interface sig, qword
	interface rev, dword 
interface_close


interface_open point
	interface x_coord, qword
	interface y_coord, qword
	interface z_coord, dword
interface_close

interface_open multiboot2
	DeriveInterface multiboot
	interface point3D, point
interface_close

interface_open multiboot
	interface mb2, multiboot2
interface_close

%define MB_ADDR 0x1000


DefineVariableCall InitialiseVGADriver, 2, 'hello.asm'

DefineCall HelloWorld, 2,'test.asm', STDCALL_GCC64



DefineVariableCall main, 2,'test.asm'
DeclareCall main, InternDef, GlobalDef, DarkSide

	mov rax, Arg_InternDef
	mov rdx, Arg_GlobalDef

	secure_call InitialiseVGADriver, 100, rax

	mov rdx, multiboot2.sig
EndFunction

MyIntok:
	dq 0, 0, 0
