.include "xc.inc"
    
.text

.global _test
    
_test:
    #mov #0x
    
    _loop:
	inc W0, W0
	cp W0, #0x5
	bra lt, _loop
	
    ;add W0, [W2], W0
    
    return
    
    