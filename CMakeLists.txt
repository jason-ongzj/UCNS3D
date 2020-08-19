cmake_minimum_required(VERSION 3.12)
project(UCNS3D CXX C)

include(CMakeDependentOption)

set(MKLROOT "/apps/software/imkl/2020.1.217-iimpi-2020a/\
compilers_and_libraries_2020.1.217/linux/mkl")
set(PARAVIEW_CRESCENT_BUILD "/apps/software/ParaView/\
5.8.0-intel-2020a-Python-3.8.2-mpi")

find_package(Python COMPONENTS Interpreter Development)
message("${Python_LIBRARIES}")
message("${Python_INCLUDE_DIRS}")
include_directories(${PYTHON_INCLUDE_DIRS})

if (NOT EXISTS ${UCNS3D_ROOT})
  message(FATAL_ERROR "Please specify a valid source directory for UCNS3D.")
  return ()
endif ()

# Compile with GCC if compiled locally
if (${COMPILE_TARGET} STREQUAL "LOCAL")
  if (NOT EXISTS ${PARAVIEW_BUILD})
    message(FATAL_ERROR "Please specify a valid build directory for Paraview.")
    return ()
  endif ()

  set(PARAVIEW_BUILD_LIB "${PARAVIEW_BUILD}/lib")

  if (NOT EXISTS ${PARAVIEW_BUILD_LIB})
    message(FATAL_ERROR "The specified build library directory does not exist.")
  else ()
    message("Paraview library found: ${PARAVIEW_BUILD_LIB}")
  endif ()

  message(STATUS "..........................................\n"
	"-- ..........................................\n"
	"-- ............Compiling with GCC............\n"
	"-- ..........................................\n"
	"-- ..........................................")

  set(BLASFLAGS "-I /usr/local/include")
  set(BLASLIBS "-L/usr/local/lib")
  set(CMAKE_Fortran_FLAGS_DEBUG "-ffree-line-length-none -fdefault-real-8 \
    -fopenmp -Wall -Wno-unused-parameter  -fbounds-check -finit-local-zero \
    -fdump-parse-tree -fdump-core -fbacktrace -fdefault-double-8 -fbackslash \
    -O0 -fcray-pointer -Wno-lto-type-mismatch ${BLASFLAGS}" CACHE STRING "")

  set(CMAKE_Fortran_FLAGS_RELEASE "-fdefault-real-8 -fdefault-double-8 \
    -fopenmp -fbackslash -ffree-line-length-none -fcray-pointer -Ofast \
    -Wno-lto-type-mismatch ${BLASFLAGS}" CACHE STRING "")

  set(CMAKE_PREFIX_PATH ${PARAVIEW_BUILD} CACHE STRING "")

else ()
  # Compile with Intel compilers if compiled on Crescent
  set(PARAVIEW_BUILD_LIB "${PARAVIEW_CRESCENT_BUILD}/lib64")
  set(PARAVIEW_DIR "${PARAVIEW_BUILD_LIB}/cmake/paraview-5.8")
  message(STATUS "Compiling UCNS3D on Crescent...")

  # Check if Paraview library directory exists
  if (NOT EXISTS ${PARAVIEW_BUILD_LIB})
    message(FATAL_ERROR "The specified build library directory does not exist.")
  else ()
    message("Paraview library found: ${PARAVIEW_BUILD_LIB}")
  endif ()

  message(STATUS "..........................................\n"
	"-- ..........................................\n"
	"-- ......Compiling with Intel compilers......\n"
	"-- ..........................................\n"
	"-- ..........................................")

  set(BLASFLAGS "-I ${MKLROOT}/include")
  set(BLASLIBS "-Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_lp64.a \
    ${MKLROOT}/lib/intel64/libmkl_sequential.a \
    ${MKLROOT}/lib/intel64/libmkl_core.a -Wl,--end-group -lpthread -lm -ldl")

  set(CMAKE_Fortran_FLAGS_DEBUG "-i4 -r8 -O0 -g -debug all -xHost -traceback \
    -qopenmp -qopenmp-link=static ${BLASFLAGS} -check bounds -fp-model strict \
    -debug inline-debug-info -zero -Wl,--no-as-needed \
    -Wl,--unresolved-symbols=ignore-in-shared-libs" CACHE STRING "")

  set(CMAKE_Fortran_FLAGS_RELEASE "-i4 -r8 -O3 -xHost -fp-model precise -zero \
    -qopenmp  -qopenmp-link=static ${BLASFLAGS}" CACHE STRING "")

  set(PYTHON_LIB "/apps/software/Python/3.8.2-GCCcore-9.3.0/lib/libpython3.8.so.1.0")
endif ()

find_package(ParaView REQUIRED)

# Check if Catalyst is enabled for Paraview
if (NOT TARGET ParaView::PythonCatalyst)
  message(STATUS
    "Skipping example: ${CMAKE_PROJECT_NAME} requires ParaView to be built "
    "with Catalyst and Python support enabled. Please rebuild ParaView (or "
    "point to a different build of ParaView) with PARAVIEW_ENABLE_PYTHON set "
    "to TRUE")
  return ()
endif ()

# Check if MPI is enabled for Paraview
if (NOT PARAVIEW_USE_MPI)
  message(STATUS
    "Skipping example: ${CMAKE_PROJECT_NAME} requires ParaView to be built "
    "with MPI support enabled. Please rebuild ParaView (or point to a different "
    "build of ParaView) with PARAVIEW_USE_MPI set to TRUE")
  return ()
endif ()

# Check if Fortran compiler exists
if (CMAKE_Fortran_COMPILER)
  enable_language(Fortran OPTIONAL)

  if (CMAKE_Fortran_COMPILER_WORKS)
    find_package(MPI REQUIRED COMPONENTS Fortran)

    # Check if Fortran MPI compiler is found
    if (NOT MPI_Fortran_FOUND)
      message(STATUS
        "Skipping example: ${CMAKE_PROJECT_NAME} requires MPI \
        support, but none was found.")
      return ()
    endif ()

    add_subdirectory(CODE)
  endif ()

endif()
