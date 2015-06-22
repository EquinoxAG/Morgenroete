%include "morgenroete.inc"


interface ThirdStruc, kollaps, word
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
	ReserveStackSpace Lokla, seq,[Mokkla.intY.tod.Mongo], Mokkla.intY.tod.kollaps
	UpdateStackPtr

	mov rax, Mokkla.intY.tod

	DestroyStack myStack
	ret

MyIntok:
	dq 0, 0, 0
