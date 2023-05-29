  .globl sq_root_compute
  .globl sq_root_compute_array
  .globl sq_root_compute_varargs

  .text
sq_root_compute:
  movl    %edi,         %esi    # Prepare n for printf
  xorl    %edx,         %edx    # result = 0
  testl   %edi,         %edi    # check if the input is 0
  je      .src_print            # early out to printf
  movl    %edi,         %ecx    # number = n
  movl    $1,           %eax    # c = 1
.src_loop:
  subl    %eax,         %ecx    # number -= c
  addl    $2,           %eax    # c += 2
  inc     %edx                  # ++result
  cmpl    %eax,         %ecx    # number - c
  jnb     .src_loop             # (number - c) >= 0
.src_print:
  movl    $format,      %edi    # 1st arg
  xorq    %rax,         %rax    # clear rax for printf
  call    printf
  ret

sq_root_compute_array:
  testl   %edi,         %edi    # test if number of elems is 0
  je      .srca_invalid
  pushq   %rbp                  # save rbp
  pushq   %rbx                  # save rbx
  subq    $8,           %rsp    # allocate 8 bytes on the stack
  movq    %rsi,         %rbx    # save the start of the array address in rbx
  movslq  %edi,         %rdi    # upcast n
  leaq    (%rsi,%rdi,4),%rbp    # save the last element in rbp  
.srca_loop:
  movl    (%rbx),       %edi    # Store current element in 1st arg for sqrt
  call    sq_root_compute
  addq    $4,           %rbx    # move the pointer to the next element
  cmpq    %rbp,         %rbx    # check if its the last element
  jne     .srca_loop
  addq    $8,           %rsp    # deallocate 8 bytes on stack
  popq    %rbx                  # restore rbx
  popq    %rbp                  # restore rbp
.srca_invalid:
  ret

sq_root_compute_varargs:
  pushq   %rbp                  # store rbp's current value
  movq    %rsp,         %rbp    # move the stack to where rbp was
  subq    $64,          %rsp    # allocate 64 bytes to store first 6 args on stack. 64 to be aligned to 16 bytes.
  movq    %rdi,     -48(%rbp)
  movq    %rsi,     -40(%rbp)
  movq    %rdx,     -32(%rbp)
  movq    %rcx,     -24(%rbp)
  movq    %r8,      -16(%rbp)
  movq    %r9,       -8(%rbp)
  leaq    -48(%rbp),    %rbx    # store the starting address
.srcv_loop:
  movq    (%rbx),       %rdi    # load the value at the current address into arg for sq_root_compute
  testq   %rdi,         %rdi    # check if value is 0
  jbe     .srcv_invalid
  cmpq    %rdi,         %rbp    # check if the value loaded was rbp. 
  jbe     .srcv_skip_return_addr
  pushq   %rbx                  # save rbx in case it is used
  call    sq_root_compute
  popq    %rbx                  # restore rbx
  addq    $8,           %rbx    # move to next address
  jmp     .srcv_loop
.srcv_skip_return_addr:
  addq    $16,           %rbx   # 16 bytes are: rbp and return address
  jmp     .srcv_loop
.srcv_invalid:
  movq    %rbp,         %rsp
  popq    %rbp
  ret

  .data
format: .asciz "Square root of %u is %u \n"
