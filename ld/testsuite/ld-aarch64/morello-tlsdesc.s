.section .tbss
.globl var2
.globl var4
.hidden var4
.globl var5
.protected var5
var2:
        .word   0
        .size   var2, .-var2

var3:
        .word   0
        .word   0
        .word   0
        .word   0
        .word   0
        .size   var3, .-var3
var4:
	.word   0
	.size   var4, .-var4
var5:
	.word   0
	.size   var5, .-var5

.section .text
.ifndef STATIC
f1:
	bl	extf
	mrs	c2, CTPIDR_EL0
	adrp	c0, :tlsdesc:var1
	ldr	c1, [c0, #:tlsdesc_lo12:var1]
	add	c0, c0, #:tlsdesc_lo12:var1
	nop
	.tlsdesccall var1
	blr	c1
.endif

f2:
	mrs	c2, CTPIDR_EL0
	adrp	c0, :tlsdesc:var2
	ldr	c1, [c0, #:tlsdesc_lo12:var2]
	add	c0, c0, #:tlsdesc_lo12:var2
	nop
	.tlsdesccall var2
	blr	c1

access_hidden:
	mrs	c2, CTPIDR_EL0
	adrp	c0, :tlsdesc:var4
	ldr	c1, [c0, #:tlsdesc_lo12:var4]
	add	c0, c0, #:tlsdesc_lo12:var4
	nop
	.tlsdesccall var2
	blr	c1

access_protected:
	mrs	c2, CTPIDR_EL0
	adrp	c0, :tlsdesc:var5
	ldr	c1, [c0, #:tlsdesc_lo12:var5]
	add	c0, c0, #:tlsdesc_lo12:var5
	nop
	.tlsdesccall var2
	blr	c1

.globl _start
.type _start, STT_FUNC
_start:
	mrs	c2, CTPIDR_EL0
	adrp	c0, :tlsdesc:var3
	ldr	c1, [c0, #:tlsdesc_lo12:var3]
	add	c0, c0, #:tlsdesc_lo12:var3
	nop
	.tlsdesccall var3
	blr	c1