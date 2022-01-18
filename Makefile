CPP = g++
DEBUG = -O2

all: pent

pent: pent.cpp
	$(CPP) $(DEBUG) -o pent pent.cpp

#---------- Testing ----------

# ${1}=width, ${2}=height, ${3}=flip, ${4}=outfile
define run
	./pent -w ${1} ${3} > sol-${1}x${2}${3}.txt
	@wc -l sol-${1}x${2}${3}.txt | \
		awk '{ print $$1 / (1 + ${2}), "solutions" }'
endef

test: pent
	$(call run,10,6,)
	$(call run,10,6,-N)
	$(call run,12,5,)
	$(call run,12,5,-N)
	$(call run,15,4,)
	$(call run,15,4,-N)
	$(call run,20,3,)
	$(call run,20,3,-N)

#---------- Clean ----------

.PHONY: clean
clean:
	$(RM) pent *.o sol-*.txt
