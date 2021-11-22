option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)

function(enable_static_analysis IS_DEBUG)

	if(IS_DEBUG EQUAL 0)

		if(ENABLE_CLANG_TIDY)
			find_program(CLANGTIDY clang-tidy)
			if(CLANGTIDY)
				set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY} -extra-arg=-Wno-unknown-warning-option)
			else()
				message(SEND_ERROR "clang-tidy requested but executable not found")
			endif()
		endif()

	endif()

endfunction()

enable_static_analysis("$<CONFIG:DEBUG>")