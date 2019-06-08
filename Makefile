VERSION = $(shell grep "define VERSION" src/filter.h | cut -d '"' -f 2)
NAME = $(shell grep "define PLUGIN" src/filter.h | cut -d '"' -f 2)

# Run vcvars32 first to setup the build toolchain
build: clean
	cmd /c msbuild DeinterlaceBlend.sln /p:Configuration=Release /t:Build

release: build
	mkdir -p dist
	zip "dist/$(NAME)-$(VERSION).zip" Release/*.dll *.sln *.filters *.vcxproj src test

clean:
	rm -rf Release

