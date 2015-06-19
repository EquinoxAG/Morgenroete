%include "morgenroete.inc"


interface ThirdStruc, kollaps, qword
interface ThirdStruc, Mongo, dword

interface MyStruc, string, dword
interface MyStruc, welt, qword
interface MyStruc, tod, ThirdStruc
interface seq, intX, dword
interface seq, intY, MyStruc


interface_constructor MyStruc, main
interface_destructor MyStruc, destu
interface_constructor seq, main
interface_destructor seq, destu


DEFINE_CALL main, 3
DEFINE_CALL destu, 1



destu:
global main
main:
	CreateStack myStack
	ReserveStackSpace Mokkla, seq, rax, rdx
	ReserveStackSpace Lokla, seq,dword[Mokkla.intY.tod.Mongo], rdx
	UpdateStackPtr

	mov rax, Mokkla.intY.tod.Mongo

	DestroyStack myStack

	ret


MyIntok:
	dq 0, 0, 0
