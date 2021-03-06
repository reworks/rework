if (WIN32)
	set(GALAXY_LINK_FLAGS_DEBUG
		/ignore:4099
		/MACHINE:X64
		/INCREMENTAL
		/DEBUG
		/SUBSYSTEM:CONSOLE
	)

	set(GALAXY_LINK_FLAGS_RELEASE
		/ignore:4099
		/MACHINE:X64
		/INCREMENTAL:NO
		/OPT:REF
		/OPT:ICF
		/SUBSYSTEM:WINDOWS
		/ENTRY:mainCRTStartup
	)
else()
	set(GALAXY_LINK_FLAGS_DEBUG
		-pthread
		-static
		-static-libgcc
		-static-libstdc++
		-pie
		-latomic
		-lstdc++fs
		-ltbb
	)

	set(GALAXY_LINK_FLAGS_RELEASE
		-pthread
		-static
		-static-libgcc
		-static-libstdc++
		-pie
		-latomic
		-lstdc++fs
		-ltbb
	)
endif()