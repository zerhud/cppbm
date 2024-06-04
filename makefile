builddir := make_build
tests_excluded_for_clang := "serp/test"
tests_excluded_for_cpp20 := "serp/test"
tests_for_cpp17 := "serp/test17"
GCC := g++ -MMD -pipe -std=gnu++23 -fwhole-program -march=native -fdiagnostics-color=always
GCC20 := g++ -MMD -pipe -std=gnu++20 -fwhole-program -march=native -fdiagnostics-color=always
GCC17 := g++ -MMD -pipe -std=gnu++17 -fwhole-program -march=native -fdiagnostics-color=always
CLANG := clang++ -MMD -pipe -std=gnu++23 -march=native -fdiagnostics-color=always
CLANG20 := clang++ -MMD -pipe -std=gnu++20 -march=native -fdiagnostics-color=always
CLANG17 := clang++ -MMD -pipe -std=gnu++17 -march=native -fdiagnostics-color=always

ifeq ($(strip $(VERBOSE)),)
	LOG := @
else
	LOG :=
endif

tests_src := $(shell find . -ipath '*/tests/*.cpp' | sed 's/^..//g')

.PHONY: all clean

base = $(basename $(subst tests/,,$(1)))
proj = $(dir $(call base,$(1)))

all: $(foreach src_file,$(tests_src),$(call base,$(src_file)))
	@echo -e "\e[7;32mAll Done\e[0m"


define create_test_dir_template
$(1):
	mkdir -p $$@
endef

define create_test_template

-include $(builddir)/$(call base,$(1))_gcc.d
$(builddir)/$(call base,$(1))_gcc: makefile $(1) | $(builddir)/$(dir $(call base,$(1)))
	$(GCC) -I$(call proj,$(1)) $(1) -o $$@

ifeq ($(findstring $(call base,$(1)),$(tests_excluded_for_cpp20)),)
-include $(builddir)/$(call base,$(1))_gcc20.d
$(builddir)/$(call base,$(1))_gcc20: makefile $(1) | $(builddir)/$(dir $(call base,$(1)))
	$(GCC20) -I$(call proj,$(1)) $(1) -o $$@
else
$(builddir)/$(call base,$(1))_gcc20: makefile $(1) | $(builddir)/$(dir $(call base,$(1)))
	@echo -e "\033[0;31mskiping for \033[1;36m$(1)\033[0;31m for cpp20\033[0m"
endif

ifeq ($(findstring $(call base,$(1)),$(tests_excluded_for_clang)),)
-include $(builddir)/$(call base,$(1))_clang.d
$(builddir)/$(call base,$(1))_clang: makefile $(1) | $(builddir)/$(dir $(call base,$(1)))
	$(CLANG) -I$(call proj,$(1)) $(1) -o $$@ && $(builddir)/$(call base,$(1))_clang
ifeq ($(findstring $(call base,$(1)),$(tests_excluded_for_cpp20)),)
-include $(builddir)/$(call base,$(1))_clang20.d
$(builddir)/$(call base,$(1))_clang20: makefile $(1) | $(builddir)/$(dir $(call base,$(1)))
	$(CLANG20) -I$(call proj,$(1)) $(1) -o $$@ && $(builddir)/$(call base,$(1))_clang
else
$(builddir)/$(call base,$(1))_clang20: makefile $(1)
	@echo -e "\033[0;31mskiping for \033[1;36m$(1)\033[0;31m for cpp20\033[0m"
endif
else
$(builddir)/$(call base,$(1))_clang: makefile $(1)
	@echo -e "\033[0;31mskiping for \033[1;36m$(1)\033[0;31m for clang\033[0m"
$(builddir)/$(call base,$(1))_clang20: makefile $(1)
	@echo -e "\033[0;31mskiping for \033[1;36m$(1)\033[0;31m for clang with cpp 20\033[0m"
endif

ifeq ($(findstring $(call base,$(1)),$(tests_for_cpp17)),)
-include $(builddir)/$(call base,$(1))_clang.d
$(builddir)/$(call base,$(1))_gcc17: makefile $(1) | $(builddir)/$(dir $(call base,$(1)))
	$(GCC17) -I$(call proj,$(1)) $(1) -o $$@ && $(builddir)/$(call base,$(1))_clang
-include $(builddir)/$(call base,$(1))_clang20.d
$(builddir)/$(call base,$(1))_clang17: makefile $(1) | $(builddir)/$(dir $(call base,$(1)))
	$(CLANG17) -I$(call proj,$(1)) $(1) -o $$@ && $(builddir)/$(call base,$(1))_clang

.PHONY: test17
test17:: $(builddir)/$(call base,$(1))_gcc17 $(builddir)/$(call base,$(1))_clang17
	$(builddir)/$(call base,$(1))_gcc17
	$(builddir)/$(call base,$(1))_clang17
endif

.PHONY: $(call base,$(1))
$(call base,$(1)): \
    $(builddir)/$(call base,$(1))_gcc \
    $(builddir)/$(call base,$(1))_gcc20 \
    $(builddir)/$(call base,$(1))_clang \
    $(builddir)/$(call base,$(1))_clang20
	$(builddir)/$(call base,$(1))_gcc
	test -f $(builddir)/$(call base,$(1))_gcc20 && $(builddir)/$(call base,$(1))_gcc20
	test -f $(builddir)/$(call base,$(1))_clang && $(builddir)/$(call base,$(1))_clang
	test -f $(builddir)/$(call base,$(1))_clang20 && $(builddir)/$(call base,$(1))_clang20

clean::
	rm -f $(builddir)/$(call base,$(1))_gcc{,.d}
	rm -f $(builddir)/$(call base,$(1))_gcc20{,.d}
	rm -f $(builddir)/$(call base,$(1))_clang{,.d}

endef

directories := $(sort $(foreach src_file,$(tests_src),$(builddir)/$(dir $(call base,$(src_file)))))
$(foreach src_file,$(directories), $(eval $(call create_test_dir_template,$(src_file))))

$(foreach src_file,$(tests_src),$(eval $(call create_test_template,$(src_file))))
