section	.rodata						
	format_string: db "%s", 10, 0										; format string
	format_number: db "%d", 10, 0										; format number
	prompt_str: db "calc: " , 10 , 0											
	operand_error: db "Error: Operand Stack Overflow"					
	args_error: db "Error: Insufficient Number of Arguments on Stack"
	stdin: dd 0			; sdtin file discreption
	stdout: dd 1		; sdtout file discreption
	stderr: dd 2		; sdterr file discreption
	
section .bss
    buffer: resb 80            		; storing input

section .data
	result: dd 0
	stack: dd 5						; allocating 20 bytes for 5 pointers
	num_of_ops: dd 0				; total number of operations

section .text
  	align 16
	global main 
	global handle_input
	extern printf 
	extern fflush
	extern malloc 
	extern calloc 
	extern free 
	extern gets 
	extern fgets 

main: 
	pushad
	pushfd

	call mycalc					; actual function
	
	popad
	popfd

	push eax					; pushing number of operations on Stack
	push format_number			; pushing format_number
	call printf					; printing 

	mov esp, ebp 				; freeing func AF
	pop ebp 					; restore AF of main
	ret

mycalc:
	.loop:
		push prompt_str
		call printf
		add esp, 4						; remove pushed argument

		push dword [stdin]              ; reading from stdin
		push dword 80                   ; max length
		push dword buffer               ; input buffer
		call fgets						; reading input
		add esp, 12                     ; remove args from stack

		cmp eax, "q"				
		je .q_case
;		cmp eax, "+"
;		je add_case
;		cmp eax, "p"
;		je pop_and_print
;;		cmp eax, "d"
;		je duplicate
;		cmp eax, "^"
;		je power
;		cmp eax, "v"
;		je negative_power
;;		cmp eax, "n"
;		je one_bits

;		.operand:

		.q_case:
			mov eax, [num_of_ops]		; return value
																			; TODO: free allocated memory
			mov esp, ebp 				; freeing func AF
			pop ebp 					; restore AF of main
			ret
