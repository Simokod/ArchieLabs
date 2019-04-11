section	.rodata						
	format_string: db "%s", 10, 0										; format string
	format_number: db "%d", 10, 0										; format number
	prompt_str:    db "calc: " , 10, 0											
	operand_error: db "Error: Operand Stack Overflow", 10, 0
	args_error:    db "Error: Insufficient Number of Arguments on Stack", 10, 0
	stack_capacity EQU 5

section .bss
    buffer: resb 80            		; storing input
	stack: resd stack_capacity		; allocating 20 bytes for 5 pointers

section .data
	current_size: db 0				; amount of operands in the stack
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
	extern fgets 
	extern stdin
	extern stdout
	extern stderr

main:
	call mycalc					; actual function
	
	push eax					; pushing number of operations on Stack
	push format_number			; pushing format_number
	call printf					; printing 
	add esp, 8					; freeing func memory

	ret

mycalc:
	push ebp					; backup last activation frame
	mov ebp, esp				; save current activation frame
	pushad						; backup registers

	main_loop:
		push prompt_str
		call printf
		add esp, 4						; remove pushed argument

		push dword [stdin]              ; reading from stdin
		push 80                   		; max length
		push buffer               		; input buffer
		call fgets						; reading input
		add esp, 12                     ; remove args from stack

switch:
		cmp byte [buffer], 'q'				
		je q_case
;		cmp byte [buffer], "+"				; dont forget to increase num_of_ops
;		je add_case
;		cmp byte [buffer], "p"
;		je pop_and_print
;		cmp byte [buffer], "d"
;		je duplicate
;		cmp byte [buffer], "^"
;		je power
;		cmp byte [buffer], "v"
;		je negative_power
;		cmp byte [buffer], "n"
;		je one_bits

		operand:
			cmp byte [current_size], stack_capacity		; checking if stack is full
			jne read_operand
			push operand_error			; pushing error message
			push format_string			; pushing format
			call printf					; printing error messsage
			add esp, 8					; remove args from stack
			jmp main_loop

		read_operand:
			mov ecx, eax				; moving pointer to buffer to ecx

			push 1						; size of char for calloc
			push 5						; num of bytes to callocate (1 for digits + 4 for pointer)
			call calloc					; allocating memory for the first link
			add esp, 8					; remove pushed argument

			mov ebx, [current_size]			; putting current_size in ebx for memory addition
			mov dword [stack+ebx*4], eax	; storing pointer to linked list in the stack
			inc byte [current_size]			; increasing amount of items in the stack

			operand_loop:
				cmp word [buffer], 0xA		; check if finished reading the number
				je end_loop					; if yes - finish

				mov edx, [ecx]				; moving current numbers to edx
				sub edx, '0'				; turning to number from ascii
				mov [eax], edx				; storing current numbers in the linked list

				mov ebx, eax				; saving address of last link
				push 1						; size of char for calloc
				push 5						; num of bytes to callocate (1 for digits + 4 for pointer)
				call calloc					; allocating memory for next link
				add esp, 8					; remove pushed argument

				mov [ebx+1], eax			; storing the next link address in current link
				jmp operand_loop

			end_loop:
				jmp main_loop

		q_case:
			mov eax, [num_of_ops]		; return value
																			; TODO: free allocated memory
			mov esp, ebp 				; freeing func AF
			pop ebp 					; restore AF of main
			ret
