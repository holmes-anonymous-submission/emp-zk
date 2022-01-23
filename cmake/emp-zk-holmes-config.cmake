find_package(emp-ot)

find_path(EMP-ZK-HOLMES_INCLUDE_DIR NAMES emp-zk-holmes/emp-zk.h)
find_library(EMP-ZK-HOLMES_LIBRARY NAMES emp-zk-holmes)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(emp-zk-holmes DEFAULT_MSG EMP-ZK-HOLMES_INCLUDE_DIR EMP-ZK-HOLMES_LIBRARY)

if(EMP-ZK-HOLMES_FOUND)
	set(EMP-ZK-HOLMES_INCLUDE_DIRS ${EMP-ZK-HOLMES_INCLUDE_DIR} ${EMP-OT_INCLUDE_DIRS})
	set(EMP-ZK-HOLMES_LIBRARIES ${EMP-OT_LIBRARIES} ${EMP-ZK-HOLMES_LIBRARY})
endif()
