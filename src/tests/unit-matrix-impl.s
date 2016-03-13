/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Design approach test program 1 (DATP_1)
 *                  └─Unit tests
 *     toolset: ISO C '99 + GAS + glib
 *     content: Test-local hvMatrixMultiply implementation for System V AMD64 ABI (text/x-asm)
 *     version: 1.0
 *
 * ARG1:RDI: *A, ARG2:RSI: *B, ARG3:RDX: *R, RCX: loop counter
 * XMM0--XMM3: matrix A, XMM4: 4x B[i,j], XMM5: row's sum
 */

	.text

	.align 32
	.globl	hvMatrixMultiply_asm
	.type	hvMatrixMultiply_asm, @function
hvMatrixMultiply_asm:
	movaps (%rdi), %xmm0
	movaps 16(%rdi), %xmm1
	movaps 32(%rdi), %xmm2
	movaps 48(%rdi), %xmm3
	movq $48, %rcx
1:
	movss (%rsi, %rcx), %xmm4
	shufps $0, %xmm4, %xmm4
	mulps %xmm0, %xmm4
	movaps %xmm4, %xmm5

	movss 4(%rsi, %rcx), %xmm4
	shufps $0, %xmm4, %xmm4
	mulps %xmm1, %xmm4
	addps %xmm4, %xmm5

	movss 8(%rsi, %rcx), %xmm4
	shufps $0, %xmm4, %xmm4
	mulps %xmm2, %xmm4
	addps %xmm4, %xmm5

	movss 12(%rsi, %rcx), %xmm4
	shufps $0, %xmm4, %xmm4
	mulps %xmm3, %xmm4
	addps %xmm4, %xmm5

	movaps %xmm5, (%rdx, %rcx)
	subq $16, %rcx
	jge 1b
	ret
	.size	hvMatrixMultiply_asm, .-hvMatrixMultiply_asm

/*
 Assembly correctness checklist:
 1. Write faultless code
		Avoid using callee-save/compiler-reserved registers (RBP, RBX, R12-15)
		Don't exceed array bounds and remember to scale array indexes
		Do not confuse variables' values with their addresses
		Disable name mangling for inline assembly blocks in C++
		Don't mix signed and unsigned integers
		Double check loop counters and exit conditions
		(e.g., remember that INC and DEC *do not* set CF)
 2. Keep track of the stack!
		Don't use relative stack addressing relaying on active stack pointer
		Keep pops/pushes paired for all execution paths
 3. Obey the ABI:
		Calling convention (e.g., f(RDI,RSI,RDX,RCX)->RAX)
		Memory aligning (e.g., 16-aligned SP becore a CALL)
 5. Clean
		Free local data
		Restore clobbered registers
		Clear MMX/YMM state
		Clean up the floating point register stack
		Clear direction flag (CLD)
		Don't forget to return :-)
*/

