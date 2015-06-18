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

	ConnectHeapToInterface MyIntok, MyStruc
	mov rax, MyIntok.welt
	
	uninterface MyStruc

	CreateStack HalloStack

	ReserveStackSpace MySt, dword
	UpdateStackPtr

	IsLocalDefined Is_Def,'%$STACK_NAfgME'


	DestroyStack HalloStack	
	ret


MyIntok:
	.welt:
	dq 0, 0, 0
