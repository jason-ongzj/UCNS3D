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
  integer :: flag
  !
  call requestdatadescription(step,time,flag)
  !
  if (flag .ne. 0) then
    call needtocreategrid(flag)
    if (flag .ne. 0) then
      call creategrid(pointSet, kmaxn, el_connect, XMPIELRANK(N), N, ISIZE)
    end if

    call addfield(scalarRU, "RU"//char(0))
    call addfield(scalarRV, "RV"//char(0))
    call addfield(qCriterion, "Q"//char(0))
    call coprocess()
  end if

  return
end subroutine

end module tcp
