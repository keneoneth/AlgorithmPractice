# TEST Candidates
# A) std::priority_queue
# B) boost::pairing_heap
# C) batch_pairing_heap_priqueue
# D) fast_concurrent_pri_skipqueue (one-thread)
# E) fast_concurrent_pri_skipqueue (mutli-thread, max 1k elements per thread)
# F) fast_concurrent_pri_skipqueue (mutli-thread, max 10k elements per thread)
# G) fast_concurrent_pri_skipqueue (mutli-thread, max 100k elements per thread)


# TEST I: add elements one by one with random priority
# TESTID:
# i) 1k elements
# ii) 10k elements
# iii) 100k elements
# iv) 1M elements
# v) 10M elements
# vi) 100M elements


# TEST II: add elements batch by batch with random priority between batches
# TESTID:
# i) 1k elements
# ii) 10k elements
# iii) 100k elements
# iv) 1M elements
# v) 10M elements
# vi) 100M elements

testmain:
	@if (test $(cand) && test $(type) && test $(id) && test $(BOOST_PATH)); then \
		g++ -Ofast testmain.cc -o testmain -DTCAND=$(cand) -DTCAND_$(cand) -DTTYPE=$(type) -DTTYPE_$(type) -DTID=$(id) -I $(BOOST_PATH) -DTID_$(id); \
	else \
		echo "arg cand/type/id is missing"; \
		exit 1; \
	fi

run : testmain
	./testmain

clean:
	@[ -f testmain ] && rm testmain || true