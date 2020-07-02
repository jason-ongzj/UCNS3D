add_executable(ucns3d_p
  FECxxAdaptor.cxx
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
  FEFortranAdaptor.f90
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
    ParaView::PythonCatalyst
    VTK::CommonDataModel
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
  
  # Paraview shared libraries on HPC
  add_library(PVPythonCatalyst SHARED IMPORTED)
  set_target_properties(PVPythonCatalyst PROPERTIES
    IMPORTED_LOCATION ${PARAVIEW_BUILD_LIB}/libvtkPVPythonCatalyst-pv5.8.so
  )

  add_library(IOLegacy SHARED IMPORTED)
  set_target_properties(IOLegacy PROPERTIES
    IMPORTED_LOCATION ${PARAVIEW_BUILD_LIB}/libvtkIOLegacy-pv5.8.so
  )

  add_library(PVCatalyst SHARED IMPORTED)
  set_target_properties(PVCatalyst PROPERTIES
    IMPORTED_LOCATION ${PARAVIEW_BUILD_LIB}/libvtkPVCatalyst-pv5.8.so
  )

  add_library(IOCore SHARED IMPORTED)
  set_target_properties(IOCore PROPERTIES
    IMPORTED_LOCATION ${PARAVIEW_BUILD_LIB}/libvtkIOCore-pv5.8.so
  )

  add_library(FiltersCore SHARED IMPORTED)
  set_target_properties(FiltersCore PROPERTIES
    IMPORTED_LOCATION ${PARAVIEW_BUILD_LIB}/libvtkFiltersCore-pv5.8.so
  )

  add_library(CommonExecutionModel SHARED IMPORTED)
  set_target_properties(CommonExecutionModel PROPERTIES
    IMPORTED_LOCATION ${PARAVIEW_BUILD_LIB}/libvtkCommonExecutionModel-pv5.8.so
  )

  add_library(CommonDataModel SHARED IMPORTED)
  set_target_properties(CommonDataModel PROPERTIES
    IMPORTED_LOCATION ${PARAVIEW_BUILD_LIB}/libvtkCommonDataModel-pv5.8.so
  )

  add_library(CommonTransforms SHARED IMPORTED)
  set_target_properties(CommonTransforms PROPERTIES
    IMPORTED_LOCATION ${PARAVIEW_BUILD_LIB}/libvtkCommonTransforms-pv5.8.so
  )

  add_library(CommonSystem SHARED IMPORTED)
  set_target_properties(CommonSystem PROPERTIES
    IMPORTED_LOCATION ${PARAVIEW_BUILD_LIB}/libvtkCommonSystem-pv5.8.so
  )

  add_library(CommonMisc SHARED IMPORTED)
  set_target_properties(CommonMisc PROPERTIES
    IMPORTED_LOCATION ${PARAVIEW_BUILD_LIB}/libvtkCommonMisc-pv5.8.so
  )

  add_library(CommonMath SHARED IMPORTED)
  set_target_properties(CommonMath PROPERTIES
    IMPORTED_LOCATION ${PARAVIEW_BUILD_LIB}/libvtkCommonMath-pv5.8.so
  )

  add_library(PythonInterpreter SHARED IMPORTED)
  set_target_properties(PythonInterpreter PROPERTIES
    IMPORTED_LOCATION ${PARAVIEW_BUILD_LIB}/libvtkPythonInterpreter-pv5.8.so
  )

  add_library(Python_3 SHARED IMPORTED)
  set_target_properties(Python_3 PROPERTIES
    IMPORTED_LOCATION ${PYTHON_LIB}
  )

  add_library(CommonCore SHARED IMPORTED)
  set_target_properties(CommonCore PROPERTIES
    IMPORTED_LOCATION ${PARAVIEW_BUILD_LIB}/libvtkCommonCore-pv5.8.so
  )

  add_library(Sys SHARED IMPORTED)
  set_target_properties(Sys PROPERTIES
    IMPORTED_LOCATION ${PARAVIEW_BUILD_LIB}/libvtksys-pv5.8.so
  )

  target_link_libraries(ucns3d_p
  PRIVATE
    MPI::MPI_Fortran ParMetis Metis TecIo
    -Wl,--start-group Mkl_Intel_Lp64 Mkl_Sequential Mkl_Core -Wl,--end-group pthread
    PVPythonCatalyst CommonDataModel IOLegacy ${BLASLIBS}
    PythonInterpreter Python_3
    IOCore PVCatalyst FiltersCore CommonExecutionModel
    CommonDataModel CommonTransforms CommonSystem
    CommonMisc CommonMath CommonCore Sys 
    dl stdc++ -Wl,-rpath-link,${PARAVIEW_BUILD_LIB}
  )

  target_include_directories(ucns3d_p
    PUBLIC
      "/apps/software/ParaView/5.8.0-intel-2020a-Python-3.8.2-mpi/include/paraview-5.8"
  )

endif ()

set_property(TARGET ucns3d_p
  PROPERTY
    LINKER_LANGUAGE Fortran
)
