CXX = g++
#DEBUG = -g
DEBUG = -O2
CPPFLAGS = $(DEBUG) -Wall -Werror

.PHONY: progs full
progs: pent symmetry sol2html
all: test_html

pent: pent.cpp

symmetry: symmetry.cpp

sol2html: sol2html.cpp

#---------- Rules ----------

# Split sol-AxB.txt into "sol A B", and
# split sol-AxB-N.txt into "sol A B -N"
splitname = $(subst x, ,$(subst N,-N,$(subst -, ,$(patsubst %.txt,%,$1))))

%.txt: pent
	./pent -w $(word 2,$(call splitname,$@)) $(word 4,$(call splitname,$@)) > $@
	@wc -l $@ | awk '{ print $$1 / (1 + $(word 3,$(call splitname,$@))), "solutions" }'

%.symm: %.txt symmetry
	./symmetry < $< > $@

%.html: %.symm sol2html
	./sol2html < $< > $@

#---------- Testing ----------

SIZES = 10x6 12x5 15x4 20x3

ALL_SOL = $(foreach size,$(SIZES),sol-$(size).txt sol-$(size)-N.txt)
ALL_SYMM = $(foreach size,$(SIZES),sol-$(size).symm sol-$(size)-N.symm)
ALL_HTML = $(foreach size,$(SIZES),sol-$(size).html sol-$(size)-N.html)

test_sol: $(ALL_SOL)

test_symm: $(ALL_SYMM)

test_html: $(ALL_HTML)

#---------- Clean ----------

.PHONY: clean
clean:
	$(RM) pent symmetry sol2html *.o
	$(RM) sol-*.txt sol-*.symm sol-*.html
