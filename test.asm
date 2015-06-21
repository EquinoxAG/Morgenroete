%include "Morgenroetev1.inc"

interface_open multiboot
	interface sig, qword
	interface rev, dword 
interface_close


interface_open point
	interface x_coord, qword
	interface y_coord, qword
	interface z_coord, qword
interface_close

interface_open multiboot2
	DeriveInterface multiboot
	interface point3D, point
interface_close

interface_open multiboot
	interface mb2, multiboot2
interface_close

destu:
global main
main:
	mov rax, multiboot.sig
	mov rbx, multiboot.rev
	mov rcx, multiboot2.rev
	


	ConnectHeapToInterface MyIntok, multiboot

	mov_ts rax, qword[ MyIntok.mb2.point3D.z_coord + MyIntok.rev ]

	uninterface multiboot
	mov rdx, multiboot2.sig
	ret

MyIntok:
	dq 0, 0, 0
