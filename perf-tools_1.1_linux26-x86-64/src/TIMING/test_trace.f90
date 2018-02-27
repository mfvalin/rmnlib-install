program test_trace
call subroutine1
call subroutine1
call subroutine1
call subroutine1
stop
end
subroutine subroutine1
integer, save :: code=-1
print *,'code=',code
code=code+1
return
end
