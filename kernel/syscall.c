/*
 * contains the implementation of all syscalls.
 */

#include <stdint.h>
#include <errno.h>

#include "util/types.h"
#include "syscall.h"
#include "string.h"
#include "process.h"
#include "util/functions.h"
#include "elf.h"
#include "spike_interface/spike_utils.h"

elf_ctx elfsymbol;
//
// implement the SYS_user_print syscall
//
ssize_t sys_user_print(const char* buf, size_t n) {
  sprint(buf);
  return 0;
}

//
// implement the SYS_user_exit syscall
//
ssize_t sys_user_exit(uint64 code) {
  sprint("User exit with code:%d.\n", code);
  // in lab1, PKE considers only one app (one process). 
  // therefore, shutdown the system when the app calls exit()
  shutdown(code);
}

int func_symbol_backtrace(uint64 ra) {
  //STT_FUNC == 18
  uint64 closest_func = 0;
  int idx = -1;
  for (int i = 0; i < elfsymbol.symbols_count; ++i) {
    if (elfsymbol.symbols[i].st_info == 18 && elfsymbol.symbols[i].st_value < ra
        && elfsymbol.symbols[i].st_value > closest_func) {
      closest_func = elfsymbol.symbols[i].st_value;
      idx = i;
    }
  }
  return idx;
}

ssize_t sys_user_backtrace(int64 depth) {
  // 获取用户栈指针，并跳过对 print_backtrace 的调用
  uint64 user_sp = current->trapframe->regs.sp + 16 + 8;
  // 继续跳过栈帧的 16 字节
  user_sp += 16;
  
  int64 current_recur_depth = 0;
  // 遍历用户栈，直到达到设定的回溯深度
  for (uint64 p = user_sp; current_recur_depth < depth; ++current_recur_depth, p += 16) {
    // 如果栈上的返回地址为 0，表示栈结束
    if (*(uint64 *) p == 0) break;
    
    // 查找给定返回地址对应的符号
    int symbol_idx = func_symbol_backtrace(*(uint64 *) p);
    if (symbol_idx == -1) {
      sprint("fail to backtrace symbol %lx\n", *(uint64 *) p);
      continue;
    }
    
    // 打印符号对应的函数名
    sprint("%s\n", &elfsymbol.strtable[elfsymbol.symbols[symbol_idx].st_name]);
  }
  return 0;
}


//
// [a0]: the syscall number; [a1] ... [a7]: arguments to the syscalls.
// returns the code of success, (e.g., 0 means success, fail for otherwise)
//

// ssize_t sys_user_backtrace(int64 depth)
// {
//   panic("try to build the construction");
// }
long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7) {
  switch (a0) {
    case SYS_user_print:
      return sys_user_print((const char*)a1, a2);
    case SYS_user_exit:
      return sys_user_exit(a1);
    case SYS_user_backtrace:
      return sys_user_backtrace(a1);
    default:
      panic("Unknown syscall %ld \n", a0);
  }
}
