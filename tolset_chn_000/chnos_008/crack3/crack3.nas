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
	MOV	AL, 0x34
	OUT	0x43,AL
	MOV	AL, 0xFF
	OUT	0x40,AL
	MOV	AL, 0xFF
	OUT	0x40,AL
	JMP	_api_end
