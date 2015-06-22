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


destu:
global main
main:
	mov rax, multiboot.sig
	mov rbx, multiboot.rev
	mov rcx, multiboot2.rev




	ConnectHeapToInterface MyIntok, multiboot

	mov_ts dword[ ((MB_ADDR->multiboot).mb2.point3D->multiboot).sig ], 1000
	mov_ts dword[ rax + multiboot.mb2 + multiboot2.point3D + multiboot.rev ], 200
	mov_ts dword[ (MyIntok->multiboot2).point3D.z_coord ], 0

	mov_ts rax, (rcx->multiboot2).point3D
	uninterface multiboot
	mov rdx, multiboot2.sig
	ret

MyIntok:
	dq 0, 0, 0
