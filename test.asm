%include "morgenroete.inc"

interface MyStruc, string, 4
interface MyStruc, welt, 8
interface MyStruc, tod, 12
interface seq, intX, 4
interface seq, intY, 4
interface_constructor MyStruc, main, 2

global main
main:
	mov rax, MyStruc.string
	mov rbx, MyStruc.welt
	mov rcx, MyStruc.tod
	mov rdx, MyStruc_size

	CreateStack myS
	ReserveStackSpace Okax, MyStruc, rax, rdx
	ReserveStackSpace Okkox, MyStruc, 100, 200
	ReserveStackSpace Olax, seq

	mov qword[ Olax.intY ], rax
	mov qword[ Okax.tod ], rbx
	DestroyStack myS
	ret


MyIntok:
	.welt:
	dq 0, 0, 0
