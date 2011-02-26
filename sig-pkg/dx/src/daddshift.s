; daddShift: shift the pathsums right
        BITS 32
        global daddShift,shiftmask,bytemask
;
; daddShift: perform a normalization shift for a row of DADD data
;
; Synopsis:
;		void daddShift(shift, bins, data);
;		int shift;					# of bits to shift
;		int rows;					# of rows to shift
;		uint32_t bins;				# of bins to shift in a row
;		uint32_t rowbins;			# total # of bins in a row
;		uint8_t *data;				# beginning of data
; Returns:
;		Nothing.
; Description:
;		Shifts an array of data the specified # of bits to the right.
; Notes:
;		The array is shifted a row at a time.
;		Efficient implementation of this function in assembler relies
;		upon quad-word alignment of the data to be shifted.
;
daddShift:
	; set up stack frame and save general registers
	enter 8,0
	pusha

	; get # of bits to shift
	mov ecx,[ebp+8]
	movd mm1,ecx

	; build the bit mask used to ensure that adjacent bins don't interfere
	; with each other.  This requires masking low bits before the shift is
	; performed
	mov al,0xff
	shl al,cl
	mov [ebp-8],al
	mov [ebp-7],al
	mov [ebp-6],al
	mov [ebp-5],al
	mov [ebp-4],al
	mov [ebp-3],al
	mov [ebp-2],al
	mov [ebp-1],al
	movq mm2,[ebp-8]

	mov eax,[ebp+12]				; get # of rows to shift

	; compute # of quadwords to shift in each row
	mov ecx,[ebp+16]				; get # of bins to shift
	add ecx,7
	shr ecx,3
	mov ebx,ecx
	mov edx,[ebp+20]				; get total # of bins in a row

	mov edi,[ebp+24]				; get ptr to data

daddShiftReset:
	mov esi,edi
daddShiftLoop:
	movq mm0,[esi]					; get next 8 bins
	pand mm0,mm2					; mask interfering bits
	psrlq mm0,mm1					; do shift
	movq [esi],mm0					; store result
	add esi,8
	loop daddShiftLoop,ecx
	dec eax
	jle daddShiftExit
	add edi,edx						; bump to next row
	mov ecx,ebx						; reload word count
	jmp daddShiftLoop

daddShiftExit:
	emms
	popa
	leave
	ret

	end
