	.text
	.cpu 430x

;;;  Test for the assembler detecting spurious size modifiers.
	
	mov.z	r1, r2
	mov.abc	r1, r2
	mov.bcd	r1, r2
	mov.wcd	r1, r2
	mov.	r1, r2
	bis.a	#8, r2

	;;  Make sure that the range checking gets #imm20 correct.
	adda    #-524289, r12
	adda	#0x180000, r10
	;;  An immediate of #524288 will not trigger an error because
	;;  positive values up to 1048575 are allowed.  This is because
	;;  assembler programmers often specify bit patterns as immediate
	;;  values in hex knowing that they will fit, despite the fact
	;;  that those same values, when viewed as integers, are out of range.
	;;  eg: adda 0xfffff, r1
	adda	#1048576, r11
	
;;; FIXME: Add more tests of assembler error detection here.

	;;  A NOP is needed *before* an EINT instruction.
	eint
	nop
	;; And *after* a DINT instruction.
	dint

	;;  Changing interrupt states in two successive instructions
	;;  might cause an interrupt to be missed.  The assembler
	;;  should warn about this, if the -mz command line option
	;;  is used.
	eint
	dint
	nop		;;  No warning needed here.
	dint
	and	#1, r11 ;; Any instruction will do, not just NOPs.
	clr r2		;; Aliases should trigger the warning too.
	mov #1, r2
	BIC #8, SR
	BIS #8, SR
	MOV.W #1, SR 
	;;  We will also get a warning if the last instruction in the file
	;;  changes the interrupt state, since this file could be linked
	;;  with another that starts with an interrupt change.

