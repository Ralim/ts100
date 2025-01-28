#!/usr/bin/env make
INFO:=top-level Makefile for IronOS - Soldering Iron Open Source Firmware Project.


### global adjustable variables

# command for "docker compose" from DOCKER env. var.
ifdef DOCKER
DOCKER_BIN:=$(DOCKER)
else
DOCKER_BIN:=
endif

# detect availability of docker
ifndef DOCKER_BIN
DOCKER_COMPOSE:=$(shell command -v docker-compose 2>/dev/null)
DOCKER_TOOL:=$(shell command -v docker 2>/dev/null)
ifdef DOCKER_COMPOSE
DOCKER_BIN:=$(DOCKER_COMPOSE)
else ifdef DOCKER_TOOL
DOCKER_BIN:=$(DOCKER_TOOL) compose
endif # DOCKER_* checks
endif # DOCKER_BIN

# command for python-based mkdocs tool
ifndef MKDOCS
MKDOCS:=mkdocs
endif

# build output related directories
ifdef OUT
OUT_DIR=$(OUT)
else
OUT_DIR=$(CURDIR)/BUILDS
endif
OUT_HEX=$(CURDIR)/source/Hexfile


### global static variables

# docker-related files
DOCKER_YML=$(CURDIR)/Env.yml
DOCKER_FILE=$(CURDIR)/scripts/IronOS.Dockerfile

# docker dependencies
DOCKER_DEPS=$(DOCKER_YML) $(DOCKER_FILE)

# compose docker-compose command
DOCKER_CMD=$(DOCKER_BIN)  -f $(DOCKER_YML)  run  --rm  builder

# MkDocs config
MKDOCS_YML=$(CURDIR)/scripts/IronOS-mkdocs.yml

# supported models
MODELS=TS100 TS80 TS80P Pinecil MHP30 Pinecilv2 S60 TS101 S60P T55 S99 # target names & dir names
MODELS_ML=Pinecil  Pinecilv2 # target names
MODELS_MULTILANG=Pinecil_multi-lang  Pinecilv2_multi-lang # dir names

# zip command (to pack artifacts)
ZIP=zip -q -j -r


### targets

# default target to show help
help:
	@echo
	@echo "Welcome!"
	@echo "This is $(INFO)"
	@echo "To read more about supported commands (aka \"targets\"), type \"make list\"."
	@echo "But if you're impatient then just type \"make docker-build\" - it will:"
	@echo "  * download, configure & start docker container"
	@echo "  * compile builds of IronOS firmware for all supported models inside that container"
	@echo "  * export generated binaries to \"scripts/ci/artefacts/\" local directory"
	@echo "Patches are welcome. Happy Hacking!"
	@echo

# target to list supported targets with additional info
list:
	@echo
	@echo "Supported top-level targets:"
	@echo "  * help               - shows short basic help"
	@echo "  * list               - this output"
	@echo "  * docker-shell       - start docker container with shell inside to work on IronOS with all tools needed"
	@echo "  * docker-build       - compile builds of IronOS for supported models inside docker container and place them to $(OUT_DIR) (set OUT env var to override: OUT=/path/to/dir make ...)"
	@echo "  * docker-clean       - delete created docker image for IronOS & its build cache objects (to free a lot of space)"
	@echo "  * docker-clean-cache - delete build cache objects of IronOS docker image EXCEPT the image itself"
	@echo "  * docker-clean-image - delete docker image for IronOS EXCEPT its build cache objects"
	@echo "  * docs               - generate \"site\"/ directory with documentation in a form of static html files using ReadTheDocs framework and $(MKDOCS_YML) local config file"
	@echo "  * docs-deploy        - generate & deploy docs online to gh-pages branch of current github repo"
	@echo "  * tests              - run set of checks, linters & tests (equivalent of github CI IronOS project settings for push trigger)"
	@echo "  * clean-build        - delete generated files & dirs produced during builds EXCEPT docker image & its build cache"
	@echo "  * clean-full         - delete generated files & dirs produced during builds INCLUDING docker image & its build cache"
	@echo ""
	@echo "NOTES on supported pass-trough targets:"
	@echo "  * main Makefile is located in source/ directory and used to build the firmware itself;"
	@echo "  * this top-level Makefile supports to call targets from source/Makefile;"
	@echo "  * if you set up development environment right on your host, then to build firmware locally, you can just type right from here:"
	@echo
	@echo " $$ make firmware-LANG_ID model=MODEL_ID"
	@echo
	@echo "Full list of current supported IDs:"
	@echo "  * LANG_ID: $(shell echo "`ls Translations/ | grep -e "^translation_.*.json$$" | sed -e 's,^translation_,,g; s,\.json$$,,g; ' | tr '\n' ' '`")"
	@echo "  * MODEL_ID: $(MODELS)"
	@echo
	@echo "For example, to make a local build of IronOS firmware for TS100 with English language, just type:"
	@echo
	@echo " $$ make firmware-EN model=TS100"
	@echo

# detect availability of docker
docker-check:
ifeq ($(DOCKER_BIN),)
	@echo "ERROR: Can't find docker-compose nor docker tool. Please, install docker and try again"
	@exit 1
else
	@true
endif

# former start_dev.sh
docker-shell: docker-check  $(DOCKER_DEPS)
	$(DOCKER_CMD)

# former build.sh
docker-build: docker-check  $(DOCKER_DEPS)
	$(DOCKER_CMD)  make  build-all

# delete docker image
docker-clean-image:
	-docker  rmi  ironos-builder:latest

# delete docker build cache objects
docker-clean-cache:
	-docker  system  prune  --filter label=ironos-builder:latest  --force

# delete docker image & cache related to IronOS container
docker-clean: docker-clean-image  docker-clean-cache

# generate docs in site/ directory (DIR for -d is relative to mkdocs.yml file location, hence use default name/location site by setting up ../site)
docs: $(MKDOCS_YML)  Documentation/*  Documentation/Flashing/*  Documentation/images/*
	$(MKDOCS)  build  -f $(MKDOCS_YML)  -d ../site

# deploy docs to gh-pages branch of current repo automagically using ReadTheDocs framework
docs-deploy: $(MKDOCS_YML)  Documentation/*  Documentation/Flashing/*  Documentation/images/*
	$(MKDOCS)  gh-deploy  -f $(MKDOCS_YML)  -d ../site

# routine check to verify documentation
test-md:
	@echo ""
	@echo "---- Checking documentation... ----"
	@echo ""
	@/bin/sh  ./scripts/deploy.sh  docs

# shell style & linter check (github CI version of shellcheck is more recent than alpine one so the latter may not catch some policies)
test-sh:
	@echo ""
	@echo "---- Checking shell scripts... ----"
	@echo ""
	@for f in `find  ./scripts  -type f  -iname "*.sh"  ! -name "flash_ts100_linux.sh"` ; do shellcheck "$${f}"; done;

# python-related tests & checks
test-py:
	@echo ""
	@echo "---- Checking python code... ----"
	@echo ""
	flake8  Translations
	black  --check  Translations
	@$(MAKE)  -C source/  Objects/host/brieflz/libbrieflz.so
	./Translations/brieflz_test.py
	./Translations/make_translation_test.py

# clang-format check for C/C++ code style
test-ccpp:
	@echo ""
	@echo "---- Checking C/C++ code... ----"
	@echo ""
	$(MAKE)  -C source/  clean  check-style

# meta target for tests & checks based on .github/workflows/push
tests: test-md  test-sh  test-py  test-ccpp
	@echo ""
	@echo "All tests & checks have been completed successfully."
	@echo ""

# former scripts/ci/buildAll.sh - all in one to build all firmware & place the produced binaries into one output directory
build-all:
	@mkdir  -p  $(OUT_DIR)
	@chmod  0777  $(OUT_DIR)
	cd  source  &&  bash  ./build.sh
	@echo "All Firmware built"
	@for model in $(MODELS); do \
		mkdir  -p  $(OUT_DIR)/$${model} ; \
		cp  -r  $(OUT_HEX)/$${model}_*.bin  $(OUT_DIR)/$${model}/ ; \
		cp  -r  $(OUT_HEX)/$${model}_*.hex  $(OUT_DIR)/$${model}/ ; \
		cp  -r  $(OUT_HEX)/$${model}_*.dfu  $(OUT_DIR)/$${model}/ ; \
	done;
	@echo "Resulting output directory: $(OUT_DIR)"

# target to build multilang supported builds for Pinecil & PinecilV2
build-multilang:
	@for modelml in $(MODELS_ML); do \
		$(MAKE)  -C source/  -j2  model=$${modelml}  firmware-multi_compressed_European  firmware-multi_compressed_Belorussian+Bulgarian+Russian+Serbian+Ukrainian  firmware-multi_Chinese+Japanese ; \
		mkdir  -p  $(OUT_DIR)/$${modelml}_multi-lang ; \
		cp  $(OUT_HEX)/$${modelml}_multi_*.bin   $(OUT_DIR)/$${modelml}_multi-lang ; \
		cp  $(OUT_HEX)/$${modelml}_multi_*.hex   $(OUT_DIR)/$${modelml}_multi-lang ; \
		cp  $(OUT_HEX)/$${modelml}_multi_*.dfu   $(OUT_DIR)/$${modelml}_multi-lang ; \
	done;
	@echo "Resulting output directory: $(OUT_DIR)"

# target to reproduce zips according to github CI settings; artifacts will be in $(OUT_DIR)/CI/*.zip
ci: tests  build-all  build-multilang
	@mkdir  -p  $(OUT_DIR)/metadata;
	@for m in $(MODELS) $(MODELS_MULTILANG); do \
		cp LICENSE scripts/LICENSE_RELEASE.md  $(OUT_DIR)/$${m}/ ; \
		$(ZIP)  $(OUT_DIR)/$${m}.zip  $(OUT_DIR)/$${m} ;           \
		./source/metadata.py  $${m}.json  $${m};                   \
		cp  $(OUT_HEX)/$${m}.json  $(OUT_DIR)/metadata;            \
	done;
	@$(ZIP)  $(OUT_DIR)/metadata.zip  $(OUT_DIR)/metadata
	@mkdir -p  $(OUT_DIR)/CI
	@mv        $(OUT_DIR)/*.zip       $(OUT_DIR)/CI
	@chmod  0777  $(OUT_DIR)/CI
	@chmod  0666  $(OUT_DIR)/CI/*.zip
	@echo "Resulting artifacts directory: $(OUT_DIR)/CI"

# pass-through target for Makefile inside source/ dir
%:
	$(MAKE)  -C source/  $@

# global clean-up target for produced/generated files inside tree
clean-build:
	$(MAKE)  -C source/  clean-all
	rm  -Rf  site
	rm  -Rf  $(OUT_DIR)

# global clean-up target
clean-full: clean-build  docker-clean

# phony targets
.PHONY:  help  list
.PHONY:  docker-check  docker-shell  docker-build  docker-clean-image  docker-clean-cache  docker-clean
.PHONY:  docs  docs-deploy
.PHONY:  test-md  test-sh  test-py  test-ccpp  tests
.PHONY:  build-all  build-multilang  ci
.PHONY:  clean-build  clean-full
