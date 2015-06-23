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

DeclareCall HelloWorld,2
	mov rax, 10
EndFunction


global main
main:





	secure_call InitialiseVGADriver, 100, rax

	mov rdx, multiboot2.sig
	ret

MyIntok:
	dq 0, 0, 0
