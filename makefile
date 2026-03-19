SHELL := /bin/bash

UNAME_S := $(shell uname -s)

TARGET := dist

ifneq (,$(findstring MINGW,$(UNAME_S)))
	RUN_TARGET := fungi.exe
else ifneq (,$(findstring MSYS,$(UNAME_S)))
	RUN_TARGET := fungi.exe
else ifneq (,$(findstring CYGWIN,$(UNAME_S)))
	RUN_TARGET := fungi.exe
else
	RUN_TARGET := fungi
endif

all: native

native: $(TARGET)

linux:
	bash build.sh linux

macos:
	bash build.sh macos

windows:
	@if [ "$(UNAME_S)" = "Linux" ]; then \
		bash build_windows_cross.sh; \
	else \
		bash build.sh windows; \
	fi

windows-cross:
	bash build_windows_cross.sh

clean:
	bash init.sh

$(TARGET):
	bash build.sh

run:
	cd dist && ./$(RUN_TARGET)

help:
	@echo "Targets:"
	@echo "  make            - Build native binary for current OS"
	@echo "  make run        - Run native binary from dist/"
	@echo "  make linux      - Force Linux native build"
	@echo "  make macos      - Force macOS native build"
	@echo "  make windows    - Build Windows .exe (native on Windows, cross on Linux)"
	@echo "  make windows-cross - Cross-build Windows .exe from Linux"
	@echo "  make clean      - Remove build artifacts"

.PHONY: all native clean run help linux macos windows windows-cross
