CPP = g++
DEBUG = -O2

all: pent sol2svg

pent: pent.cpp
	$(CPP) $(DEBUG) -o pent pent.cpp

sol2svg: sol2svg.cpp
	$(CPP) $(DEBUG) -o sol2svg sol2svg.cpp

#---------- Rules ----------

# Split sol-AxB.txt into "sol A B", and
# split sol-AxB-N.txt into "sol A B -N"
splitname = $(subst x, ,$(subst N,-N,$(subst -, ,$(patsubst %.txt,%,$1))))

%.txt: pent
	./pent -w $(word 2,$(call splitname,$@)) $(word 4,$(call splitname,$@)) > tmp.txt
	mv tmp.txt $@
	@wc -l $@ | awk '{ print $$1 / (1 + $(word 3,$(call splitname,$@))), "solutions" }'

%.html: %.txt sol2svg
	./sol2svg < $< > $@

#---------- Testing ----------

SIZES = 10x6 12x5 15x4 20x3

SOL_TXT = $(foreach size,$(SIZES),sol-$(size).txt sol-$(size)-N.txt)
SVG_TXT = $(foreach size,$(SIZES),sol-$(size).html sol-$(size)-N.html)

test: $(SOL_TXT)

test_svg: $(SVG_TXT)

#---------- Clean ----------

.PHONY: clean
clean:
	$(RM) pent sol2svg *.o sol-*.txt tmp.txt sol-*.html
