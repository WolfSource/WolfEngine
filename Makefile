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

GIT_HOOKS_COMMIT_MSG = .git/hooks/commit-msg
$(GIT_HOOKS_COMMIT_MSG):
	cp .githooks/commit-msg $@

GIT_HOOKS_PRE_COMMIT = .git/hooks/pre-commit
$(GIT_HOOKS_PRE_COMMIT):
	cp .githooks/pre-commit $@

GIT_HOOKS_PRE_PUSH = .git/hooks/pre-push
$(GIT_HOOKS_PRE_PUSH):
	cp .githooks/pre-push $@

GIT_HOOKS_PREPARE_COMMIT_MSG = .git/hooks/prepare-commit-msg
$(GIT_HOOKS_PREPARE_COMMIT_MSG):
	cp .githooks/prepare-commit-msg $@

GIT_HOOKS = $(GIT_HOOKS_COMMIT_MSG) $(GIT_HOOKS_PRE_COMMIT) $(GIT_HOOKS_PRE_PUSH) $(GIT_HOOKS_PREPARE_COMMIT_MSG)

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
	if [ "${DETECTED_OS}" = "Darwin" ]; then \
		bash -c "$(curl --fail --location --show-error --silent https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)" \
		brew install --quiet \
			cmake \
			glib \
		\
		&& export PKG_CONFIG_PATH=$(pkg-config --variable pc_path pkg-config) \
		&& export PKG_CONFIG_PATH="/Library/Frameworks/GStreamer.framework/Versions/Current/lib/pkgconfig${PKG_CONFIG_PATH:+:}${PKG_CONFIG_PATH}" \
		; \
	elif [ "${DETECTED_OS}" = "Linux" ]; then \
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
			\
		&& apt-get autoremove --yes \
		&& apt-get clean --yes \
		&& rm -fr /tmp/* /var/lib/apt/lists/* /var/tmp/* \
		; \
	else \
		echo "Please install cmake on ${DETECTED_OS}" \
		; \
	fi

# gstreamer1.0-doc is not available on debian bullseye

.PHONY: add-fmt
add-fmt: ## Add the fmt
	rustup component add rustfmt

.PHONY: add-git-configs
add-git-configs: ## Add the git configs
	git config --global branch.autosetuprebase always; \
	git config --global color.branch true; \
	git config --global color.diff true; \
	git config --global color.interactive true; \
	git config --global color.status true; \
	git config --global color.ui true; \
	git config --global commit.gpgsign true; \
	git config --global core.autocrlf input; \
	git config --global core.editor "code --wait"; \
	git config --global diff.tool code; \
	git config --global difftool.code.cmd "code --diff \$$LOCAL \$$REMOTE --wait"; \
	git config --global gpg.program gpg; \
	git config --global init.defaultbranch main; \
	git config --global log.date relative; \
	git config --global merge.tool code; \
	git config --global mergetool.code.cmd "code --wait \$$MERGED"; \
	git config --global pull.default current; \
	git config --global pull.rebase true; \
	git config --global push.default current; \
	git config --global rebase.autostash true; \
	git config --global rerere.enabled true; \
	git config --global stash.showpatch true; \
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
	rustup toolchain install nightly; \
	rustup +nightly component add miri; \
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
	find . -type f -name *.prof* -exec rm -fr {} +; \
	rm -fr $(COVERAGE_DIR); \
	rm -fr coverage

.PHONY: clean-doc
clean-doc: ## Clean doc
	rm -fr $(DOCUMENTATION_DIR); \
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
	mkdir -p $(COVERAGE_DIR); \
	mkdir -p coverage; \
	RUSTC_BOOTSTRAP=1 RUSTFLAGS="-Zinstrument-coverage" $(CARGO_BUILD); \
	RUSTC_BOOTSTRAP=1 RUSTFLAGS="-Zinstrument-coverage" LLVM_PROFILE_FILE="$(PACKAGE)-%p-%m.profraw" $(CARGO_TEST); \
	if [ "$(GRCOV_COVERAGE_TYPE)" = "cobertura" ]; then \
		grcov . \
			--binary-path $(BIN_DIR) \
			--branch \
			--guess-directory-when-missing \
			--ignore "/*" \
			--ignore-not-existing \
			--output-path $(COVERAGE_DIR)/coverage.xml \
			--output-type cobertura \
			--source-dir . \
		;\
	elif [ "$(GRCOV_COVERAGE_TYPE)" = "lcov" ]; then \
		grcov . \
			--binary-path $(BIN_DIR) \
			--branch \
			--guess-directory-when-missing \
			--ignore "/*" \
			--ignore-not-existing \
			--output-path $(COVERAGE_DIR)/lcov.info \
			--output-type lcov \
			--source-dir . \
		;\
	elif [ "$(GRCOV_COVERAGE_TYPE)" = "html" ]; then \
		grcov . \
			--binary-path $(BIN_DIR) \
			--branch \
			--guess-directory-when-missing \
			--ignore "/*" \
			--ignore-not-existing \
			--output-path $(COVERAGE_DIR) \
			--output-type html \
			--source-dir . \
		;\
	else \
		echo "Unknown coverage type: $(GRCOV_COVERAGE_TYPE)" \
		;\
	fi; \
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
	$(CARGO_DOC); \
	mkdir -p documentation; \
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
	summarize summarize $(PACKAGE)-{$(PID)

.PHONY: miri
miri: add-miri ## Miri
	$(CARGO_MIRI)

.PHONY: release
release: $(BIN) clean-release ## Release
	mkdir -p release; \
	cp $(BIN) release/$(BIN_NAME); \
	$(STRIP) release/$(BIN_NAME); \
	shasum --algorithm 256 release/$(BIN_NAME) \
		| awk 'BEGIN { FS = " " }; { print $$1 }' > release/$(BIN_NAME).sha256

.PHONY: run
run: fetch ## Run
	$(CARGO_RUN)

.PHONY: test
test: add-fmt fetch ## Test
	$(CARGO_TEST)



# Usage: rustc [OPTIONS] INPUT
#
# Options:
#     -h, --help          Display this message
#         --cfg SPEC      Configure the compilation environment
#     -L [KIND=]PATH      Add a directory to the library search path. The
#                         optional KIND can be one of dependency, crate, native,
#                         framework, or all (the default).
#     -l [KIND=]NAME      Link the generated crate(s) to the specified native
#                         library NAME. The optional KIND can be one of
#                         static, framework, or dylib (the default).
#         --crate-type [bin|lib|rlib|dylib|cdylib|staticlib|proc-macro]
#                         Comma separated list of types of crates
#                         for the compiler to emit
#         --crate-name NAME
#                         Specify the name of the crate being built
#         --edition 2015|2018|2021
#                         Specify which edition of the compiler to use when
#                         compiling code.
#         --emit [asm|llvm-bc|llvm-ir|obj|metadata|link|dep-info|mir]
#                         Comma separated list of types of output for the
#                         compiler to emit
#         --print [crate-name|file-names|sysroot|target-libdir|cfg|target-list|target-cpus|target-features|relocation-models|code-models|tls-models|target-spec-json|native-static-libs]
#                         Compiler information to print on stdout
#     -g                  Equivalent to -C debuginfo=2
#     -O                  Equivalent to -C opt-level=2
#     -o FILENAME         Write output to <filename>
#         --out-dir DIR   Write output to compiler-chosen filename in <dir>
#         --explain OPT   Provide a detailed explanation of an error message
#         --test          Build a test harness
#         --target TARGET Target triple for which the code is compiled
#     -W, --warn OPT      Set lint warnings
#     -A, --allow OPT     Set lint allowed
#     -D, --deny OPT      Set lint denied
#     -F, --forbid OPT    Set lint forbidden
#         --cap-lints LEVEL
#                         Set the most restrictive lint level. More restrictive
#                         lints are capped at this level
#     -C, --codegen OPT[=VALUE]
#                         Set a codegen option
#     -V, --version       Print version info and exit
#     -v, --verbose       Use verbose output
#
# Additional help:
#     -C help             Print codegen options
#     -W help             Print 'lint' options and default settings
#     --help -v           Print the full set of options rustc accepts



# CARGO_RUSTDOC = $(CARGO) rustdoc --all-features --frozen --package $(PACKAGE) $(RELEASE) --target $(TARGET)

# rustdoc [options] <input>
#
# Options:
#     -h, --help          show this help message
#     -V, --version       print rustdoc's version
#     -v, --verbose       use verbose output
#     -r, --input-format [rust]
#                         the input type of the specified file
#     -w, --output-format [html]
#                         the output type to write
#     -o, --output PATH   where to place the output
#         --crate-name NAME
#                         specify the name of this crate
#         --crate-type [bin|lib|rlib|dylib|cdylib|staticlib|proc-macro]
#                         Comma separated list of types of crates
#                         for the compiler to emit
#     -L, --library-path DIR
#                         directory to add to crate search path
#         --cfg           pass a --cfg to rustc
#         --extern NAME[=PATH]
#                         pass an --extern to rustc
#         --extern-html-root-url NAME=URL
#                         base URL to use for dependencies; for example,
#                         "std=/doc" links std::vec::Vec to
#                         /doc/std/vec/struct.Vec.html
#         --plugin-path DIR
#                         removed
#     -C, --codegen OPT[=VALUE]
#                         pass a codegen option to rustc
#         --passes PASSES list of passes to also run, you might want to pass it
#                         multiple times; a value of `list` will print available
#                         passes
#         --plugins PLUGINS
#                         removed
#         --no-defaults   don't run the default passes
#         --document-private-items
#                         document private items
#         --document-hidden-items
#                         document items that have doc(hidden)
#         --test          run code examples as tests
#         --test-args ARGS
#                         arguments to pass to the test runner
#         --test-run-directory PATH
#                         The working directory in which to run tests
#         --target TRIPLE target triple to document
#         --markdown-css FILES
#                         CSS files to include via <link> in a rendered Markdown
#                         file
#         --html-in-header FILES
#                         files to include inline in the <head> section of a
#                         rendered Markdown file or generated documentation
#         --html-before-content FILES
#                         files to include inline between <body> and the content
#                         of a rendered Markdown file or generated documentation
#         --html-after-content FILES
#                         files to include inline between the content and
#                         </body> of a rendered Markdown file or generated
#                         documentation
#         --markdown-before-content FILES
#                         files to include inline between <body> and the content
#                         of a rendered Markdown file or generated documentation
#         --markdown-after-content FILES
#                         files to include inline between the content and
#                         </body> of a rendered Markdown file or generated
#                         documentation
#         --markdown-playground-url URL
#                         URL to send code snippets to
#         --markdown-no-toc
#                         don't include table of contents
#     -e, --extend-css PATH
#                         To add some CSS rules with a given file to generate
#                         doc with your own theme. However, your theme might
#                         break if the rustdoc's generated HTML changes, so be
#                         careful!
#     -Z FLAG             internal and debugging options (only on nightly build)
#         --sysroot PATH  Override the system root
#         --playground-url URL
#                         URL to send code snippets to, may be reset by
#                         --markdown-playground-url or
#                         `#![doc(html_playground_url=...)]`
#         --display-warnings
#                         to print code warnings when testing doc
#         --crate-version VERSION
#                         crate version to print into documentation
#         --sort-modules-by-appearance
#                         sort modules by where they appear in the program,
#                         rather than alphabetically
#         --default-theme THEME
#                         Set the default theme. THEME should be the theme name,
#                         generally lowercase. If an unknown default theme is
#                         specified, the builtin default is used. The set of
#                         themes, and the rustdoc built-in default, are not
#                         stable.
#         --default-setting SETTING[=VALUE]
#                         Default value for a rustdoc setting (used when
#                         "rustdoc-SETTING" is absent from web browser Local
#                         Storage). If VALUE is not supplied, "true" is used.
#                         Supported SETTINGs and VALUEs are not documented and
#                         not stable.
#         --theme FILES   additional themes which will be added to the generated
#                         docs
#         --check-theme FILES
#                         check if given theme is valid
#         --resource-suffix PATH
#                         suffix to add to CSS and JavaScript files, e.g.,
#                         "light.css" will become "light-suffix.css"
#         --edition EDITION
#                         edition to use when compiling rust code (default:
#                         2015)
#         --color auto|always|never
#                         Configure coloring of output:
#                         auto = colorize, if output goes to a tty (default);
#                         always = always colorize output;
#                         never = never colorize output
#         --error-format human|json|short
#                         How errors and other messages are produced
#         --json CONFIG   Configure the structure of JSON diagnostics
#         --disable-minification
#                         Disable minification applied on JS files
#     -W, --warn OPT      Set lint warnings
#     -A, --allow OPT     Set lint allowed
#     -D, --deny OPT      Set lint denied
#     -F, --forbid OPT    Set lint forbidden
#         --cap-lints LEVEL
#                         Set the most restrictive lint level. More restrictive
#                         lints are capped at this level. By default, it is at
#                         `forbid` level.
#         --index-page PATH
#                         Markdown file to be used as index page
#         --enable-index-page
#                         To enable generation of the index page
#         --static-root-path PATH
#                         Path string to force loading static files from in
#                         output pages. If not set, uses combinations of '../'
#                         to reach the documentation root.
#         --disable-per-crate-search
#                         disables generating the crate selector on the search
#                         box
#         --persist-doctests PATH
#                         Directory to persist doctest executables into
#         --show-coverage
#                         calculate percentage of public items with
#                         documentation
#         --enable-per-target-ignores
#                         parse ignore-foo for ignoring doctests on a per-target
#                         basis
#         --runtool The tool to run tests with when building for a different target than host

#         --runtool-arg One (of possibly many) arguments to pass to the runtool

#         --test-builder PATH
#                         The rustc-like binary to use as the test builder
#         --check         Run rustdoc checks
#         --generate-redirect-map
#                         Generate JSON file at the top level instead of
#                         generating HTML redirection files
#         --print [unversioned-files]
#                         Rustdoc information to print on stdout
#         --emit [unversioned-shared-resources,toolchain-shared-resources,invocation-specific]
#                         Comma separated list of types of output for rustdoc to
#                         emit
#
#     @path               Read newline separated options from `path`
#
# More information available at https://doc.rust-lang.org/rustdoc/what-is-rustdoc.html
