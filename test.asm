%include "morgenroete.inc"

interface MyStruc, string, 4
interface MyStruc, welt, 8
interface MyStruc, tod, 12
interface seq, intX, 4
interface seq, intY, 4

global main
main:
	mov rax, MyStruc.string
	mov rbx, MyStruc.welt
	mov rcx, MyStruc.tod
	mov rdx, MyStruc_size

	CreateStack myS
	ReserveStackSpace Okax, dword
	ReserveStackSpace Olax, qword
	FindAndReplaceLocalVar mov qword[ Olax ], orax
	DeleteStack myS
	ret


MyIntok:
	.welt:
	dq 0, 0, 0
