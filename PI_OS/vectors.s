;@--
;@-- TK-PIE
;@-- Copyright (c) 2016 - Fabio Belavenuto & Victor Trucco
;@--
;@-- All rights reserved
;@--
;@-- Redistribution and use in source and synthezised forms, with or without
;@-- modification, are permitted provided that the following conditions are met:
;@--
;@-- Redistributions of source code must retain the above copyright notice,
;@-- this list of conditions and the following disclaimer.
;@--
;@-- Redistributions in synthesized form must reproduce the above copyright
;@-- notice, this list of conditions and the following disclaimer in the
;@-- documentation and/or other materials provided with the distribution.
;@--
;@-- Neither the name of the author nor the names of other contributors may
;@-- be used to endorse or promote products derived from this software without
;@-- specific prior written permission.
;@--
;@-- THIS CODE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;@-- AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
;@-- THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
;@-- PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE
;@-- LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;@-- CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;@-- SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;@-- INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
;@-- CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;@-- ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
;@-- POSSIBILITY OF SUCH DAMAGE.
;@--
;@-- You are responsible for any legal issues arising from your use of this code.
;@--
;@---------------------------------------------------------------------------------

.globl _start
_start:
    ldr pc,reset_handler
    ldr pc,undefined_handler
    ldr pc,swi_handler
    ldr pc,prefetch_handler
    ldr pc,data_handler
    ldr pc,unused_handler
    ldr pc,irq_handler
    ldr pc,fiq_handler
reset_handler:      .word reset
undefined_handler:  .word hang
swi_handler:        .word hang
prefetch_handler:   .word hang
data_handler:       .word hang
unused_handler:     .word hang
irq_handler:        .word irq
fiq_handler:        .word fiq

reset:
    mov r0,#0x8000
    mov r1,#0x0000
    ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
    stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}
    ldmia r0!,{r2,r3,r4,r5,r6,r7,r8,r9}
    stmia r1!,{r2,r3,r4,r5,r6,r7,r8,r9}

    ;@ (PSR_IRQ_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xD2
    msr cpsr_c,r0
    mov sp,#0x8000

    ;@ (PSR_FIQ_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xD1
    msr cpsr_c,r0
    mov sp,#0x4000

    ;@ (PSR_SVC_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xD3
    msr cpsr_c,r0
    mov sp,#0x8000000

    ;@ SVC MODE, IRQ ENABLED, FIQ DIS
    ;@mov r0,#0x53
    ;@msr cpsr_c, r0

	;@ ---------- Start L1 Cache
	
	;@ R0 = System Control Register
	mrc p15,0,r0,c1,c0,0 
	
	;@ Data Cache (Bit 2)
	orr r0,#0x0004 
	
	;@ Branch Prediction (Bit 11)
	orr r0,#0x0800 
	
	;@ Instruction Caches (Bit 12)
	orr r0,#0x1000 
	
	;@ System Control Register = R0
	mcr p15,0,r0,c1,c0,0 
	;@ ------------------- 
	
	;@ ---------- Enable VFP
	;@ r1 = Access Control Register
	mrc p15, #0, r1, c1, c0, #2 
	
	;@ enable full access for p10,11
	orr r1, r1, #(0xf << 20) 
	
	;@ Access Control Register = r1
	mcr p15, #0, r1, c1, c0, #2 
	mov r1, #0
	
	;@ flush prefetch buffer because of FMXR below
	mcr p15, #0, r1, c7, c5, #4 

	;@ and CP 10 & 11 were only just enabled
	;@ Enable VFP itself
	mov r0,#0x40000000
	
     fmxr fpexc,r0
	;@ ------------------- 
	
    bl notmain
hang: b hang

.globl PUT32
PUT32:
    str r1,[r0]
    bx lr

.globl PUT16
PUT16:
    strh r1,[r0]
    bx lr

.globl PUT8
PUT8:
    strb r1,[r0]
    bx lr

.globl GET32
GET32:
    ldr r0,[r0]
    bx lr

.globl GETPC
GETPC:
    mov r0,lr
    bx lr

.globl BRANCHTO
BRANCHTO:
    bx r0

.globl dummy
dummy:
    bx lr

.globl enable_irq
enable_irq:
    mrs r0,cpsr
    bic r0,r0,#0x80
    msr cpsr_c,r0
    bx lr

irq:
    push {lr}
    bl c_irq_handler
    pop  {lr}
    subs pc,lr,#4
	
.globl enable_fiq
enable_fiq:
    mrs r0,cpsr
    bic r0,r0,#0x40
    msr cpsr_c,r0
    bx lr

fiq:
    push {r0,r1,r2,r3,lr}
    bl c_fiq_handler
    pop  {r0,r1,r2,r3,lr}
    subs pc,lr,#4



