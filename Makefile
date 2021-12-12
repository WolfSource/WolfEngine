.DEFAULT_GOAL := help
SHELL = /bin/bash

# ARCHVARIANT: arm64, armv7, amd64
# CARGO: cargo, cross
# DENY_CHECK_WHICH: advisories, bans, licenses, sources
# GRCOV_COVERAGE_TYPE: cobertura, lcov, html
# MESSAGE_FORMAT: human, json
# MIRI_SUB_COMMAND: run, test
# PACKAGE: wolf_system
# RELEASE: --release
# STRIP: aarch64-linux-gnu-strip, arm-linux-gnueabihf-strip, strip
# TARGET: aarch64-unknown-linux-musl, armv7-unknown-linux-musleabihf, x86_64-unknown-linux-musl
# VERSION: 0.1.0

ARCHVARIANT ?= $(shell rustup show | sed -n "s/^Default host: \(.*\)/\1/p" | awk 'BEGIN { FS = "-" }; { print $$1 }')
CARGO ?= cargo
DENY_CHECK_WHICH ?= advisories bans licenses sources
GRCOV_COVERAGE_TYPE ?= lcov
MESSAGE_FORMAT ?= human
PACKAGE ?= wolf_system
MIRI_SUB_COMMAND ?= run
# RELEASE ?= --release
STRIP ?= strip
TARGET ?= $(shell rustup show | sed -n "s/^Default host: \(.*\)/\1/p")
VERSION ?= 0.1.0

TARGET_DIR := target/$(TARGET)
BIN_DIR := $(TARGET_DIR)/debug
ifdef RELEASE
	BIN_DIR := $(TARGET_DIR)/release
endif

BIN := $(BIN_DIR)/$(PACKAGE)
BIN_NAME := $(PACKAGE)-v$(VERSION)-$(ARCHVARIANT)

COVERAGE_DIR := $(TARGET_DIR)/cov
DETECTED_OS := $(shell uname 2>/dev/null || echo "Windows")
DOCUMENTATION_DIR := $(TARGET_DIR)/doc

CARGO_BENCH = $(CARGO) bench --all-features --all-targets --frozen --message-format $(MESSAGE_FORMAT) --package $(PACKAGE) $(RELEASE) --target $(TARGET)
CARGO_BUILD = $(CARGO) build --all-features --all-targets --frozen --message-format $(MESSAGE_FORMAT) --package $(PACKAGE) $(RELEASE) --target $(TARGET)
CARGO_CHECK = $(CARGO) check --all-features --all-targets --frozen --message-format $(MESSAGE_FORMAT) --package $(PACKAGE) $(RELEASE) --target $(TARGET)
CARGO_CLEAN = $(CARGO) clean --frozen --package $(PACKAGE) $(RELEASE) --target $(TARGET)
CARGO_DOC = $(CARGO) doc --document-private-items --frozen --no-default-features --message-format $(MESSAGE_FORMAT) --package $(PACKAGE) $(RELEASE) --target $(TARGET)
CARGO_FETCH = $(CARGO) fetch --locked --target $(TARGET)
CARGO_FIX = $(CARGO) fix --all-features --frozen --message-format $(MESSAGE_FORMAT) --package $(PACKAGE) $(RELEASE) --target $(TARGET)
CARGO_RUN = $(CARGO) run --all-features --frozen --message-format $(MESSAGE_FORMAT) --package $(PACKAGE) $(RELEASE) --target $(TARGET)
CARGO_RUSTC = $(CARGO) +nightly rustc --all-features --frozen --message-format $(MESSAGE_FORMAT) --package $(PACKAGE) $(RELEASE) --target $(TARGET)
CARGO_RUSTDOC = $(CARGO) rustdoc --all-features --frozen --message-format $(MESSAGE_FORMAT) --package $(PACKAGE) $(RELEASE) --target $(TARGET)
CARGO_TEST = $(CARGO) test --all-features --all-targets --frozen --message-format $(MESSAGE_FORMAT) --package $(PACKAGE) $(RELEASE) --target $(TARGET)

CARGO_AUDIT = $(CARGO) audit
CARGO_CLIPPY = $(CARGO) clippy --all-features --all-targets --frozen --message-format $(MESSAGE_FORMAT) --workspace -- \
	--deny clippy::all \
	--deny clippy::cargo \
	--deny clippy::nursery \
	--deny clippy::pedantic \
	--deny warnings \
	--allow clippy::multiple_crate_versions \
	\
	--allow clippy::module_name_repetitions
CARGO_DENY = $(CARGO) deny --all-features --format $(MESSAGE_FORMAT) --workspace
CARGO_FMT = $(CARGO) fmt --all
ifeq ("$(MIRI_SUB_COMMAND)", "run")
	CARGO_MIRI = $(CARGO) +nightly miri run --all-features --frozen --package $(PACKAGE) $(RELEASE) --target $(TARGET)
else
	CARGO_MIRI = $(CARGO) +nightly miri test --all-features --all-targets --frozen --package $(PACKAGE) $(RELEASE) --target $(TARGET)
endif

$(BIN): add-fmt fetch
	$(CARGO_BUILD)

GIT_HOOKS = .git/hooks/commit-msg .git/hooks/pre-commit .git/hooks/pre-push .git/hooks/prepare-commit-msg

$(GIT_HOOKS): .git/hooks/%: .githooks/%

.githooks/%:
	touch $@

.git/hooks/%:
	cp $< $@

.PHONY: add-audit
add-audit: ## Add the audit
	$(CARGO) install cargo-audit

.PHONY: add-clippy
add-clippy: ## Add the clippy
	rustup component add clippy

.PHONY: add-conventional-commits-linter
add-conventional-commits-linter: ## Add the conventional commits linter
	$(CARGO) install --locked conventional_commits_linter

.PHONY: add-deny
add-deny: ## Add the deny
	$(CARGO) install --locked cargo-deny

.PHONY: add-os-dependency
add-os-dependency: ## Add the os dependency
ifeq ("$(DETECTED_OS)", "Darwin")
	bash -c "$(curl --fail --location --show-error --silent https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)" \
	&& brew install --quiet \
		cmake \
		glib \
	&& PKG_CONFIG_PATH=$(pkg-config --variable pc_path pkg-config) \
	&& PKG_CONFIG_PATH="/Library/Frameworks/GStreamer.framework/Versions/Current/lib/pkgconfig${PKG_CONFIG_PATH:+:}${PKG_CONFIG_PATH}" \
	&& echo "export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}" >> ~/.zshenv
else ifeq ("$(DETECTED_OS)", "Linux")
	export DEBIAN_FRONTEND="noninteractive" \
	&& apt-get update \
	&& apt-get install --no-install-recommends --yes \
		cmake \
		\
		gstreamer1.0-alsa \
		gstreamer1.0-gl \
		gstreamer1.0-gtk3 \
		gstreamer1.0-libav \
		gstreamer1.0-plugins-bad \
		gstreamer1.0-plugins-base \
		gstreamer1.0-plugins-good \
		gstreamer1.0-plugins-ugly \
		gstreamer1.0-pulseaudio \
		gstreamer1.0-qt5 \
		gstreamer1.0-tools \
		gstreamer1.0-x \
		libgstreamer-plugins-bad1.0-dev \
		libgstreamer-plugins-base1.0-dev \
		libgstreamer1.0-dev \
		\
		libges-1.0-dev \
		libgstrtspserver-1.0-dev \
	&& apt-get autoremove --yes \
	&& apt-get clean --yes \
	&& rm -fr /tmp/* /var/lib/apt/lists/* /var/tmp/*
else
	echo "Please install dependencies on $(DETECTED_OS)"
endif

# gstreamer1.0-doc is not available on debian bullseye

.PHONY: add-fmt
add-fmt: ## Add the fmt
	rustup component add rustfmt

.PHONY: add-git-configs
add-git-configs: ## Add the git configs
	git config --global branch.autosetuprebase always
	git config --global color.branch true
	git config --global color.diff true
	git config --global color.interactive true
	git config --global color.status true
	git config --global color.ui true
	git config --global commit.gpgsign true
	git config --global core.autocrlf input
	git config --global core.editor "code --wait"
	git config --global diff.tool code
	git config --global difftool.code.cmd "code --diff \$$LOCAL \$$REMOTE --wait"
	git config --global gpg.program gpg
	git config --global init.defaultbranch main
	git config --global log.date relative
	git config --global merge.tool code
	git config --global mergetool.code.cmd "code --wait \$$MERGED"
	git config --global pull.default current
	git config --global pull.rebase true
	git config --global push.default current
	git config --global rebase.autostash true
	git config --global rerere.enabled true
	git config --global stash.showpatch true
	git config --global tag.gpgsign true

.PHONY: add-git-hooks
add-git-hooks: clean-git-hooks $(GIT_HOOKS) ## Add the git hooks

.PHONY: add-grcov
add-grcov: ## Add the grcov
	$(CARGO) install --locked grcov

.PHONY: add-llvm-tools-preview
add-llvm-tools-preview: ## Add the llvm tools preview
	rustup component add llvm-tools-preview

.PHONY: add-measureme
add-measureme: ## Add the measureme
	$(CARGO) install --git https://github.com/rust-lang/measureme --tag 10.0.0 crox flamegraph stack_collapse summarize

.PHONY: add-miri
add-miri: ## Add the miri
	rustup toolchain install nightly
	rustup +nightly component add miri
	$(CARGO) +nightly miri setup

.PHONY: add-target
add-target: ## Add a target
	rustup target add $(TARGET)

.PHONY: audit
audit: add-audit ## Audit
	$(CARGO_AUDIT)

.PHONY: bench
bench: add-fmt fetch ## Bench
	$(CARGO_BENCH)

.PHONY: build
build: clean-build $(BIN) ## Build

.PHONY: check
check: add-fmt fetch ## Check
	$(CARGO_CHECK)

.PHONY: clean
clean: clean-coverage clean-doc clean-profile clean-release ## Clean
	rm -fr target

.PHONY: clean-build
clean-build: fetch ## Clean build
	$(CARGO_CLEAN)

.PHONY: clean-coverage
clean-coverage: ## Clean coverage
	find . -type f -name *.prof* -exec rm -fr {} +
	rm -fr $(COVERAGE_DIR)
	rm -fr coverage

.PHONY: clean-doc
clean-doc: ## Clean doc
	rm -fr $(DOCUMENTATION_DIR)
	rm -fr documentation

.PHONY: clean-git-hooks
clean-git-hooks: ## Clean git hooks
	rm -fr $(GIT_HOOKS)

.PHONY: clean-profile
clean-profile: ## Clean profile
	rm -fr profile

.PHONY: clean-release
clean-release: ## Clean release
	rm -fr release

.PHONY: clippy
clippy: add-clippy add-fmt fetch ## Clippy
	$(CARGO_CLIPPY)

.PHONY: conventional-commits-linter
conventional-commits-linter: add-conventional-commits-linter ## Conventional commits linter
	conventional_commits_linter --allow-angular-type-only --from-stdin

.PHONY: coverage
coverage: add-fmt add-grcov add-llvm-tools-preview clean-coverage fetch ## Coverage
	mkdir -p $(COVERAGE_DIR)
	mkdir -p coverage
	RUSTC_BOOTSTRAP=1 RUSTFLAGS="-Zinstrument-coverage" $(CARGO_BUILD)
	RUSTC_BOOTSTRAP=1 RUSTFLAGS="-Zinstrument-coverage" LLVM_PROFILE_FILE="$(PACKAGE)-%p-%m.profraw" $(CARGO_TEST)
ifeq ("$(GRCOV_COVERAGE_TYPE)", "cobertura")
	grcov . \
		--binary-path $(BIN_DIR) \
		--branch \
		--guess-directory-when-missing \
		--ignore "/*" \
		--ignore-not-existing \
		--output-path $(COVERAGE_DIR)/coverage.xml \
		--output-type cobertura \
		--source-dir .
else ifeq ("$(GRCOV_COVERAGE_TYPE)", "lcov")
	grcov . \
		--binary-path $(BIN_DIR) \
		--branch \
		--guess-directory-when-missing \
		--ignore "/*" \
		--ignore-not-existing \
		--output-path $(COVERAGE_DIR)/lcov.info \
		--output-type lcov \
		--source-dir .
else ifeq ("$(GRCOV_COVERAGE_TYPE)", "html")
	grcov . \
		--binary-path $(BIN_DIR) \
		--branch \
		--guess-directory-when-missing \
		--ignore "/*" \
		--ignore-not-existing \
		--output-path $(COVERAGE_DIR) \
		--output-type html \
		--source-dir .
else
	echo "Unknown coverage type: $(GRCOV_COVERAGE_TYPE)"
endif
	cp -R $(COVERAGE_DIR)/* coverage

.PHONY: deny-check
deny-check: add-deny ## Deny check
	$(CARGO_DENY) check $(DENY_CHECK_WHICH)

.PHONY: deny-fetch
deny-fetch: add-deny ## Deny fetch
	$(CARGO_DENY) fetch

.PHONY: deny-fix
deny-fix: add-deny ## Deny fix
	$(CARGO_DENY) fix

.PHONY: doc
doc: add-fmt clean-doc fetch ## Doc
	$(CARGO_DOC)
	mkdir -p documentation
	cp -R $(DOCUMENTATION_DIR)/* documentation

.PHONY: fetch
fetch: Cargo.lock add-target ## Fetch
	$(CARGO_FETCH)

.PHONY: fix
fix: fetch ## Fix
	$(CARGO_FIX)

.PHONY: fmt
fmt: add-fmt ## FMT
	$(CARGO_FMT)

.PHONY: fmt-check
fmt-check: add-fmt ## FMT check
	$(CARGO_FMT) -- --check

.PHONY: generate-lockfile
generate-lockfile: ## Generate lockfile
	$(CARGO) generate-lockfile

.PHONY: git
git: add-git-configs add-git-hooks ## Add git configs & hooks

.PHONY: help
help: ## Help
	@grep --extended-regexp '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) \
		| sort \
		| awk 'BEGIN { FS = ":.*?## " }; { printf "\033[36m%-33s\033[0m %s\n", $$1, $$2 }'

.PHONY: measureme
measureme: add-measureme ## Measureme flamegraph
	RUSTFLAGS="-Zself-profile=profile -Zself-profile-events=default,args" $(CARGO_RUSTC)

.PHONY: measureme-crox
measureme-crox: measureme ## Measureme crox
	crox --dir profile --minimum-duration 2

.PHONY: measureme-flamegraph
measureme-flamegraph: measureme ## Measureme flamegraph
	flamegraph $(PACKAGE)-$(PID)

.PHONY: measureme-stack-collapse
measureme-stack-collapse: measureme ## Measureme stack collapse
	stack_collapse $(PACKAGE)-$(PID)

.PHONY: measureme-summarize
measureme-summarize: measureme ## Measureme summarize
	summarize summarize $(PACKAGE)-$(PID)

.PHONY: miri
miri: add-miri ## Miri
	$(CARGO_MIRI)

.PHONY: release
release: $(BIN) clean-release ## Release
	mkdir -p release
	cp $(BIN) release/$(BIN_NAME)
	$(STRIP) release/$(BIN_NAME)
	shasum --algorithm 256 release/$(BIN_NAME) \
		| awk 'BEGIN { FS = " " }; { print $$1 }' > release/$(BIN_NAME).sha256

.PHONY: run
run: fetch ## Run
	$(CARGO_RUN)

.PHONY: test
test: add-fmt fetch ## Test
	$(CARGO_TEST)
