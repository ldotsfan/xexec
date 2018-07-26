handle SIGSEGV nostop noprint
define hook-quit
 set confirm off
end
alias ir = info registers
alias it = info threads
alias taa = thread apply all
