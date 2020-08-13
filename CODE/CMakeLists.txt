add_executable(ucns3d_p
  vtkCPVTKPipeline.cxx
  vtkCPVTKPipeline.h
  FEDataStructures.cxx
  FEDataStructures.h
  FEAdaptor.cxx
  FEAdaptor.h
  FEDriver.cxx
  declarations.f90
  translate.f90
  grid_t.f90
  flow_operations.f90
  memory.f90
  io.f90
  mpi_p.f90
  communications.f90
  riemann.f90
  flux_p.f90
  profile.f90
  init_p.f90
  parts.f90
  bc_p.f90
  matrix.f90
  grid_p.f90
  basis.f90
  gradients.f90
  prestore.f90
  reconstruct_p.f90
  source.f90
  implicit_fluxes.f90
  implicit_time.f90
  flux_pv.f90
  time_p.f90
  der_r.f90
  svd.f90
  local_pt.f90
  main.f90
  libparmetis.a
  libmetis.a
  libtecio.a
)

# Static libraries
add_library(ParMetis STATIC IMPORTED)
set_target_properties(ParMetis PROPERTIES
  IMPORTED_LOCATION ${UCNS3D_ROOT}/libparmetis.a
)

add_library(Metis STATIC IMPORTED)
set_target_properties(Metis PROPERTIES
  IMPORTED_LOCATION ${UCNS3D_ROOT}/libmetis.a
)

add_library(TecIo STATIC IMPORTED)
set_target_properties(TecIo PROPERTIES
  IMPORTED_LOCATION ${UCNS3D_ROOT}/libtecio.a
)

# Link libraries based on compiling on local or HPC machine
if (${COMPILE_TARGET} STREQUAL "LOCAL")
  target_link_libraries(ucns3d_p
  PRIVATE
    ${MPI_Fortran_LIBRARIES}  ParMetis Metis TecIo -no-pie
    VTK::CommonDataModel
    VTK::CommonCore
    VTK::IOLegacy
    VTK::FiltersCore
    ${BLASLIBS} openblas
  )

else ()

  # Static MKL libraries on HPC
  add_library(Mkl_Intel_Lp64 STATIC IMPORTED)
  set_target_properties(Mkl_Intel_Lp64 PROPERTIES
    IMPORTED_LOCATION ${MKLROOT}/lib/intel64/libmkl_intel_lp64.a
  )

  add_library(Mkl_Sequential STATIC IMPORTED)
  set_target_properties(Mkl_Sequential PROPERTIES
    IMPORTED_LOCATION ${MKLROOT}/lib/intel64/libmkl_sequential.a
  )

  add_library(Mkl_Core STATIC IMPORTED)
  set_target_properties(Mkl_Core PROPERTIES
    IMPORTED_LOCATION ${MKLROOT}/lib/intel64/libmkl_core.a
  )

  target_link_libraries(ucns3d_p
  PRIVATE
    MPI::MPI_Fortran ParMetis Metis TecIo -no-pie
    -Wl,--start-group Mkl_Intel_Lp64 Mkl_Sequential Mkl_Core
    -Wl,--end-group pthread
    VTK::CommonDataModel
    VTK::CommonCore
    VTK::IOLegacy
    VTK::FiltersCore
    ${BLASLIBS} dl stdc++ -Wl,-rpath-link,${PARAVIEW_BUILD_LIB}
  )

  target_include_directories(ucns3d_p
    PUBLIC
      "/apps/software/ParaView/5.8.0-intel-2020a-Python-3.8.2-mpi/include/paraview-5.8"
  )
endif ()

target_link_libraries(ucns3d_p
  PRIVATE
    ParaView::Catalyst
    ParaView::VTKExtensionsCore
    VTK::IOImage
    VTK::RenderingAnnotation
    VTK::RenderingVolume
    VTK::RenderingCore
    VTK::FiltersGeometry
    VTK::CommonColor
    VTK::ParallelMPI
    VTK::ParallelCore
    VTK::RenderingParallel
)

set_property(TARGET ucns3d_p
  PROPERTY
    LINKER_LANGUAGE Fortran
)
