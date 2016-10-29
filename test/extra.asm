// extra.asm
total=0
a=12+45

if a != 0 then
    call add( a, 3)
endif

if (a == 0) then
    call add( total, 3)
else
    call add( total, -3)
endif

for i=0 to 5
    print total
next i 

print total

end

sub add(x, y)
   total=x+y
endsub             
