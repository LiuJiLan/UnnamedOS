.text			
.global	_start
_start:
auipc t0, 0x0200U
jalr x1, t0, 0x0
