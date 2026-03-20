SHELL := /bin/bash

all: windows

windows:
	@if [ "$(shell uname -s)" = "Linux" ]; then \
		bash build_windows_cross.sh; \
	else \
		bash build.sh windows; \
	fi

windows-cross:
	bash build_windows_cross.sh

clean:
	bash init.sh

help:
	@echo "Targets:"
	@echo "  make windows      - Build Windows .exe"
	@echo "  make windows-cross - Cross-build Windows .exe from Linux"
	@echo "  make clean        - Remove build artifacts"

.PHONY: all clean help windows windows-cross
