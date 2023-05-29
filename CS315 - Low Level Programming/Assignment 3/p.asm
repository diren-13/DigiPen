poly:
  movl    4(%esp), %edx
  movl    16(%esp), %ecx
  fldl    8(%esp)
  fldl    (%edx)
  testl   %ecx, %ecx
  jle     .L6
  leal    8(%edx), %eax
  fld     %st(1)
  leal    8(%edx,%ecx,8), %edx
.L3:
  fldl    (%eax)
  addl    $8, %eax
  fmul    %st(1), %st
  faddp   %st, %st(2)
  fmul    %st(2), %st
  cmpl    %edx, %eax
  jne     .L3
  fstp    %st(0)
  fstp    %st(1)
  jmp     .L1
.L6:
  fstp    %st(1)
.L1:
  ret