module tcp
  use iso_c_binding
  use DECLARATION
  USE MPIINFO
  implicit none
  public :: coprocess_grid
    contains

subroutine coprocess_grid(step,time)
  use iso_c_binding
  implicit none

  integer, intent(in) :: step
  real*8, intent(in) :: time
  integer :: flag, var = 2
  !
  call requestdatadescription(step,time,flag)
  !
  if (flag .ne. 0) then
    call needtocreategrid(flag)
    if (flag .ne. 0) then
      call gridfunction(pointSet, kmaxn, el_connect, XMPIELRANK(N), N)
      call testfunction(pointSet, kmaxn, el_connect, XMPIELRANK(N), N, ISIZE)
    end if

    call densityfunction(scalarR);
    call scalarsfunction(scalarRU);
    call addfield(scalarR, "R"//char(0))
    call addfield(scalarRU, "RU"//char(0))
    call addfield(scalarRV, "RV"//char(0))
    call addfield(scalarE, "E"//char(0))
    call coprocess()
  end if

  return
end subroutine

end module tcp
