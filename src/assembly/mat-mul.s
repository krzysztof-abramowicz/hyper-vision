/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *                  └─Matrix module
 *     toolset: ISO C '99 + GAS x86-64
 *     content: hvMatrixMultiplyASM implementation for System V AMD64 ABI (text/x-asm)
 *     version: 1.0 OS
 *
 * Algorithm uses row-major intuition adjusted for column-major arrays
 * (i.e. operands were swapped, since A^T × B^T = (B × A)^T )
 *
 * ARG1:RDI: *A, ARG2:RSI: *B, ARG3:RDX: *R, RCX: loop counter
 * XMM0--XMM3: matrix A, XMM4: 4x B[i,j], XMM5: row's sum
 */

	.text
	.align 32
	.globl	hvMatrixMultiplyASM
	.type	hvMatrixMultiplyASM, @function
hvMatrixMultiplyASM:
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
	.size	hvMatrixMultiplyASM, .-hvMatrixMultiplyASM

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
		don't use relative stack addressing relaying on active stack pointer
		keep pops/pushes paired for all execution paths
 3. Obey the ABI:
		calling convention (e.g., f(RDI,RSI,RDX,RCX)->RAX)
		memory aligning (e.g., 16-aligned SP becore a CALL)
 5. Clean
		free local data
		restore clobbered registers
		clear MMX/YMM state
		clean up the floating point register stack
		clear direction flag (CLD)
		don't forget to return :-)
*/

