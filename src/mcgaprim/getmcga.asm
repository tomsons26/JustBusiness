IDEAL
P386
MODEL USE32 FLAT

global C Get_MCGA_Selector :NEAR
global C Get_Standard_Selector :NEAR

PROC Get_MCGA_Selector C Near

push    edx
xor     eax, eax
mov     ax, cs
or      ax, 7
mov     dx, 0x1C
or      ax, dx
pop     edx
retn

ENDP

PROC Get_Standard_Selector C Near

push    edx
xor     eax, eax
mov     ax, ds
pop     edx
retn

ENDP
