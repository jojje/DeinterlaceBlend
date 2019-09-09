.PHONY: dist release

VERSION = $(shell grep "define VERSION" src/filter.h | cut -d '"' -f 2)
NAME = $(shell grep "define PLUGIN" src/filter.h | cut -d '"' -f 2)
ARCHIVE = dist/$(NAME)-$(VERSION).zip

# Run vcvars32 first to setup the build toolchain
build: clean
	cmd //c msbuild DeinterlaceBlend.sln //p:Configuration=Release //p:Platform="x86" //t:Build
	cmd //c msbuild DeinterlaceBlend.sln //p:Configuration=Release //p:Platform="x64" //t:Build

release: build
	@if [ -e "$(ARCHIVE)" ]; then echo "Release $(ARCHIVE) alrady exists!" && exit 1; fi
	mkdir -p dist
	zip -r $(ARCHIVE) Release/*/*.dll *.sln *.filters *.vcxproj src test

clean:
	rm -rf Release

