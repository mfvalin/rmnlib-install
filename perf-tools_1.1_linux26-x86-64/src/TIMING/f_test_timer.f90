program stuck
integer timeout
integer i

timeout=10
call start_dead_process_timer(timeout)
do while(1.eq.1)
i=i+1
enddo
stop
end
