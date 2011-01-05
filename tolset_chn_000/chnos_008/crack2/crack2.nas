[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
	EXTERN	_api_end
[FILE "crack1.ca"]
[SECTION .text]
	GLOBAL	_CHNMain
_CHNMain:
	MOV	EAX,1*8
	MOV	DS,AX
	MOV	BYTE [0x102600],0
	JMP	_api_end
