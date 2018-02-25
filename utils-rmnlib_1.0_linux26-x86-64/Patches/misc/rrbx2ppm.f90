program rrbx2ppm
! Author M. Valin
use iso_c_binding
implicit none

interface
  integer(C_INT) function c_write(fd,buf,count) BIND(C,name='write')
  use iso_c_binding
  integer (C_INT), value ::fd,count
  type (C_PTR), value :: buf
  end function c_write
end interface

integer nbr,nr,lcp,ncp
character *80 comment
integer, pointer, dimension(:,:) :: arr
byte, pointer, dimension(:,:) :: rgb
integer nargs
integer i,j,k
character *4096 infile, outfile
integer linfile,istat,loutfile,ostat,iframe
integer left,cur1,cur2,cur3,pos,comp

byte     RED(0:7), GREEN(0:7), BLUE(0:7)
! replace former value of 255 with -1, compatible with signed integer
DATA RED   /  -1, 000,  -1, 000,  -1,  -1, 000, 000 /
DATA GREEN /  -1, 000, 000,  -1,  -1, 000,  -1, 000 /
DATA BLUE  /  -1, 000, 000,  -1, 000,  -1, 000,  -1 /

iframe=0
nargs=command_argument_count()
istat=0
call get_command_argument(1,infile,linfile,istat)
ostat=0
!call get_command_argument(2,outfile,loutfile,ostat)

if(istat.eq.0)write(0,*)'infile="',trim(infile),'"'
if(istat.eq.0)write(0,*)'linfile=',linfile
!if(ostat.eq.0)write(0,*)' outfile="',trim(outfile),'"'
!if(ostat.eq.0)write(0,*)'loutfile=',loutfile
if(istat.ne.0 .or. ostat.ne.0) then
  write(0,*) 'ERROR, input file or output file is missing'
  stop
endif
!
! read RRBX header
!
open(unit=1,file=trim(infile),action='READ',form='UNFORMATTED')
read(1)nbr,nr,lcp,ncp
read(1)comment
write(0,*)'nbr,nr,lcp,ncp=',nbr,nr,lcp,ncp
write(0,*)'"',trim(comment),'"'
!
! process rasters
!
allocate(arr(lcp,max(3,ncp)))  ! allocate a raster (ncp color planes)
arr=0
allocate(rgb(3,nbr))
1 continue
write(6,'(A,/,A,A,/,2I8,/,I8)')'P6','#',trim(comment),nbr,nr,255
call flush(6)
read(1,end=2)comment
write(0,*)'"',trim(comment),'"'
do k=1,nr
  read(1)((arr(i,j),i=1,lcp),j=1,ncp)  ! read ncp color plane raster
  cur1=arr(1,1)
  cur2=arr(1,2)
  cur3=arr(1,3)
  pos=1
  left=32
  do i=1,nbr ! convert 3 color plane raster to RGB for PPM
    cur1=ishftc(cur1,1)
    cur2=ishftc(cur2,1)
    cur3=ishftc(cur3,1)
    comp=iand(cur1,1)  ! build color index from 3 color planes
    comp=ior( comp , ishft( iand(cur2,1) , 1) )
    comp=ior( comp , ishft( iand(cur3,1) , 2) )
    rgb(1,i)=red(comp)    ! convert to RBG
    rgb(2,i)=green(comp)
    rgb(3,i)=blue(comp)
    left=left-1
    if(left.eq.0)then
      pos=pos+1
      left=32
      cur1=arr(pos,1)
      cur2=arr(pos,2)
      cur3=arr(pos,3)
    endif
  enddo
  i=c_write(1,c_loc(rgb(1,1)),nbr*3)
!  print *,k
enddo
iframe=iframe+1
write(0,*) 'end of frame',iframe
2 continue
close(1)
stop
end
