[FORMAT "WCOFF"]
[INSTRSET "i486p"]
[OPTIMIZE 1]
[OPTION 1]
[BITS 32]
[FILE "crack4.ca"]
[SECTION .text]
	GLOBAL	_CHNMain
_CHNMain:
	CLI
fin:
	HLT
	JMP	fin
